#include "brepch.h"
#include "GizmoService_Gem.h"

#include "Bruno/Platform/DirectX/Device.h"
#include "Bruno/Platform/DirectX/GraphicsContext_Gem.h"
#include "Bruno/Platform/DirectX/GraphicsPipelineState.h"
#include "Bruno/Platform/DirectX/RootSignature_Gem.h"
#include "Bruno/Platform/DirectX/ShaderCompiler.h"
#include "Bruno/Platform/DirectX/Shader_Gem.h"
#include "Bruno/Renderer/PrimitiveBatch_Gem.h"

namespace Bruno::DX
{
    GizmoService::GizmoService(GraphicsDevice* device, Camera& camera) : 
        m_device(device),
        m_camera(camera),
        m_primitiveBatch(device)
    {
        for (size_t i = 0; i < 3; i++)
        {
            //m_activeAxisColors[i] = m_axisColors[i];
        }
        m_selectionState.m_gizmoWorldMatrix = Math::Matrix::Identity;
        m_selectionState.m_rotationMatrix = Math::Matrix::Identity;
        m_selectionState.m_gizmoObjectOrientedWorld = Math::Matrix::Identity;
        m_selectionState.m_gizmoAxisAlignedWorld = Math::Matrix::Identity;
        m_selectionState.m_screenScaleMatrix = Math::Matrix::Identity;
        m_selectionState.m_gizmoPosition = Math::Vector3::Zero;
        m_selectionState.m_isDragging = false;
        //m_translationScaleSnapDelta = Math::Vector3::Zero;

        //m_sceneGizmoCamera = m_camera;
        //m_sceneGizmoCamera.SetLens(1.0f, 10.0f);
        UpdateLocalState();
    }

    void GizmoService::Initialize()
    {
        // 1. Inicializar buffers internos de la geometría procedimental
        //m_primitiveBatch.Begin();
        
        // 2. Compilar/Cargar Shaders Unlit sencillos para Gizmos
        DX::ShaderCompiler compiler; 

        // Compilas usando DXC (nota el _6_0)
        auto vertexShaderByteCode = compiler.CompileFromFile(L"Shaders/UnlitColor.hlsl", L"VS", L"vs_6_0");
        auto pixelShaderByteCode  = compiler.CompileFromFile(L"Shaders/UnlitColor.hlsl", L"PS", L"ps_6_0");
		
        // 16 floats equivalen a una Matriz de 4x4
        CD3DX12_ROOT_PARAMETER gizmoParams[1];
        gizmoParams[0].InitAsConstants(16, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

        // Inicializamos la firma sin samplers
        m_rootSignature = std::make_unique<DX::RootSignature>(*m_device);
        m_rootSignature->Initialize(1, gizmoParams);

        // 1. Input Layout EXCLUSIVO para Gizmos (Position + Color)
        D3D12_INPUT_ELEMENT_DESC inputLayout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        // 2. Llenar el descriptor
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputLayout, _countof(inputLayout) };
        psoDesc.pRootSignature = m_rootSignature->GetNative();
        psoDesc.VS = { reinterpret_cast<BYTE*>(vertexShaderByteCode->GetBufferPointer()), vertexShaderByteCode->GetBufferSize() };
        psoDesc.PS = { reinterpret_cast<BYTE*>(pixelShaderByteCode->GetBufferPointer()), pixelShaderByteCode->GetBufferSize() };

        // 3. Rasterizer para Gizmos (Sin Culling para que siempre se vean)
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // CRÍTICO para primitivas sueltas
    
        // 4. Depth Stencil para "Rayos X" o Gizmos sobrepuestos
        psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = TRUE;
        psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO; // No escriben en el Z-Buffer
        psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;     // Siempre dibujan por encima

        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
        psoDesc.SampleDesc.Count = 1;

        // 5. Instanciar y configurar la clase genérica
        m_psoDepthOff = std::make_unique<GraphicsPipelineState>(*m_device);
        m_psoDepthOff->Initialize(psoDesc);
    }

    void GizmoService::BuildGeometry(uint32_t frameIndex)
    {
        m_primitiveBatch.Begin();

        if (m_currentGizmoType == GizmoType::None || !m_isActive)
        {
            m_primitiveBatch.End(frameIndex);
            return;
        }

        Math::Matrix baseMatrix = (m_transformSpace == TransformSpace::Local || m_currentGizmoType == GizmoType::Scale) 
                                      ? m_selectionState.m_gizmoObjectOrientedWorld 
                                      : m_selectionState.m_gizmoAxisAlignedWorld;

        // 1. Extraemos los valores de tu antigua estructura de configuración (RenderConfig/GizmoConfig)
        const float stickHeight   = m_gizmoConfig.StickHeight;
        const float lineOffset    = m_gizmoConfig.LineOffset;
        const float stickRadius   = m_gizmoConfig.StickRadius;
        const float arrowHeight   = m_gizmoConfig.ArrowheadHeight;
        const float arrowRadius   = m_gizmoConfig.ArrowheadRadius;
        const int   slices        = m_gizmoConfig.Tessellation;

        // 2. Pre-calculamos las matemáticas para adaptar tu legacy a las nuevas primitivas
        // El cilindro mide la diferencia entre la altura total y el hueco inicial
        float cylinderLength = stickHeight - lineOffset;
    
        // Como DrawCylinder dibuja desde el centro, su centro Y local debe estar a la mitad de su recorrido
        float cylinderCenterY = lineOffset + (cylinderLength * 0.5f);
    
        // El cono empieza exactamente donde termina el stick
        float coneBaseY = stickHeight;

        switch (m_currentGizmoType)
        {
        case GizmoType::Translation:
            {
                // ==========================================
                // EJE Y (Verde - Up)
                // ==========================================
                Math::Matrix matY = baseMatrix;
            
                Math::Matrix cylMatY = Math::Matrix::CreateTranslation(0.0f, cylinderCenterY, 0.0f) * matY;
                m_primitiveBatch.DrawCylinder(cylMatY, cylinderLength, stickRadius, slices, m_axisColors[1]);
            
                Math::Matrix coneMatY = Math::Matrix::CreateTranslation(0.0f, coneBaseY, 0.0f) * matY;
                m_primitiveBatch.DrawCone(coneMatY, arrowHeight, arrowRadius, slices, m_axisColors[1]);

                // ==========================================
                // EJE X (Rojo - Right) 
                // Rotamos -90° en Z para que el Y local apunte al X global
                // ==========================================
                Math::Matrix matX = Math::Matrix::CreateRotationZ(-Math::PI / 2.0f) * baseMatrix;
            
                Math::Matrix cylMatX = Math::Matrix::CreateTranslation(0.0f, cylinderCenterY, 0.0f) * matX;
                m_primitiveBatch.DrawCylinder(cylMatX, cylinderLength, stickRadius, slices, m_axisColors[0]);
            
                Math::Matrix coneMatX = Math::Matrix::CreateTranslation(0.0f, coneBaseY, 0.0f) * matX;
                m_primitiveBatch.DrawCone(coneMatX, arrowHeight, arrowRadius, slices, m_axisColors[0]); // Corregí el m_axisColors[01] que tenías

                // ==========================================
                // EJE Z (Azul - Forward)
                // Rotamos +90° en X para que el Y local apunte al Z global
                // ==========================================
                Math::Matrix matZ = Math::Matrix::CreateRotationX(Math::PI / 2.0f) * baseMatrix;
            
                Math::Matrix cylMatZ = Math::Matrix::CreateTranslation(0.0f, cylinderCenterY, 0.0f) * matZ;
                m_primitiveBatch.DrawCylinder(cylMatZ, cylinderLength, stickRadius, slices, m_axisColors[2]);
            
                Math::Matrix coneMatZ = Math::Matrix::CreateTranslation(0.0f, coneBaseY, 0.0f) * matZ;
                m_primitiveBatch.DrawCone(coneMatZ, arrowHeight, arrowRadius, slices, m_axisColors[2]);

                // ==========================================
                // PLANOS DE TRASLACIÓN (XY, XZ, YZ)
                // ==========================================
                // Estos son los cuadritos que permiten mover en 2 ejes a la vez.
                // Usaremos el grosor y offset de tu legacy: MULTI_AXIS_THICKNESS
                const float planeSize = m_gizmoConfig.MultiAxisThickness; // O el nombre que le des
                const float planeOffset = lineOffset + (planeSize * 0.5f);
            
                // Plano XY (Azul, porque excluye Z)
                Math::Matrix planeXY = Math::Matrix::CreateTranslation(planeOffset, planeOffset, 0.0f) * baseMatrix;
                // DrawBox necesita una escala plana (ej: Z casi 0)
                Math::Matrix scaleXY = Math::Matrix::CreateScale(planeSize, planeSize, 0.01f) * planeXY;
                m_primitiveBatch.DrawBox(scaleXY, 1.0f, Math::Color(m_axisColors[2].R(), m_axisColors[2].G(), m_axisColors[2].B(), 0.5f)); // Semitransparente

                // Plano XZ (Verde, porque excluye Y)
                Math::Matrix planeXZ = Math::Matrix::CreateTranslation(planeOffset, 0.0f, planeOffset) * baseMatrix;
                Math::Matrix scaleXZ = Math::Matrix::CreateScale(planeSize, 0.01f, planeSize) * planeXZ;
                m_primitiveBatch.DrawBox(scaleXZ, 1.0f, Math::Color(m_axisColors[1].R(), m_axisColors[1].G(), m_axisColors[1].B(), 0.5f));

                // Plano YZ (Rojo, porque excluye X)
                Math::Matrix planeYZ = Math::Matrix::CreateTranslation(0.0f, planeOffset, planeOffset) * baseMatrix;
                Math::Matrix scaleYZ = Math::Matrix::CreateScale(0.01f, planeSize, planeSize) * planeYZ;
                m_primitiveBatch.DrawBox(scaleYZ, 1.0f, Math::Color(m_axisColors[0].R(), m_axisColors[0].G(), m_axisColors[0].B(), 0.5f));

                break;
            }
        case GizmoType::Scale:
            {
                // 1. Extraemos los valores de configuración (igual que en Translation para mantener simetría)
                const float stickHeight   = m_gizmoConfig.StickHeight;
                const float lineOffset    = m_gizmoConfig.LineOffset;
                const float stickRadius   = m_gizmoConfig.StickRadius;
                const int   slices        = m_gizmoConfig.Tessellation;

                // En tu legacy, el tamaño de la caja se basaba en la altura de la flecha * 0.85f
                const float boxSize       = m_gizmoConfig.ArrowheadHeight * 0.85f;

                // 2. Pre-calculamos las distancias para las primitivas
                float cylinderLength  = stickHeight - lineOffset;
                float cylinderCenterY = lineOffset + (cylinderLength * 0.5f);
    
                // Centramos la caja justo en la punta del stick
                float boxCenterY      = stickHeight;

                // ==========================================
                // EJE Y (Verde - Up)
                // ==========================================
                Math::Matrix matY = baseMatrix;
    
                Math::Matrix cylMatY = Math::Matrix::CreateTranslation(0.0f, cylinderCenterY, 0.0f) * matY;
                m_primitiveBatch.DrawCylinder(cylMatY, cylinderLength, stickRadius, slices, m_axisColors[1]);
    
                Math::Matrix boxMatY = Math::Matrix::CreateTranslation(0.0f, boxCenterY, 0.0f) * matY;
                m_primitiveBatch.DrawBox(boxMatY, boxSize, m_axisColors[1]);

                // ==========================================
                // EJE X (Rojo - Right)
                // ==========================================
                Math::Matrix matX = Math::Matrix::CreateRotationZ(-Math::PI / 2.0f) * baseMatrix;
    
                Math::Matrix cylMatX = Math::Matrix::CreateTranslation(0.0f, cylinderCenterY, 0.0f) * matX;
                m_primitiveBatch.DrawCylinder(cylMatX, cylinderLength, stickRadius, slices, m_axisColors[0]);
    
                Math::Matrix boxMatX = Math::Matrix::CreateTranslation(0.0f, boxCenterY, 0.0f) * matX;
                m_primitiveBatch.DrawBox(boxMatX, boxSize, m_axisColors[0]);

                // ==========================================
                // EJE Z (Azul - Forward)
                // ==========================================
                Math::Matrix matZ = Math::Matrix::CreateRotationX(Math::PI / 2.0f) * baseMatrix;
    
                Math::Matrix cylMatZ = Math::Matrix::CreateTranslation(0.0f, cylinderCenterY, 0.0f) * matZ;
                m_primitiveBatch.DrawCylinder(cylMatZ, cylinderLength, stickRadius, slices, m_axisColors[2]);
    
                Math::Matrix boxMatZ = Math::Matrix::CreateTranslation(0.0f, boxCenterY, 0.0f) * matZ;
                m_primitiveBatch.DrawBox(boxMatZ, boxSize, m_axisColors[2]);

                // ==========================================
                // CENTRO: ESCALA UNIFORME (EJE XYZ)
                // ==========================================
                // Dibujamos un cubo central (generalmente gris o blanco) para escalar proporcionalmente
                // El tamaño suele ser ligeramente mayor al lineOffset para que sea fácil de clickear
                float centerBoxSize = lineOffset * 1.5f;
                Math::Color centerColor = Math::Color(0.8f, 0.8f, 0.8f, 1.0f); // Gris claro
                m_primitiveBatch.DrawBox(baseMatrix, centerBoxSize, centerColor);

                // Opcional: Planos 2D (XY, XZ, YZ)
                // Si tu motor soporta escalar en 2 ejes a la vez, copiaríamos la lógica de los planos
                // del GizmoType::Translation aquí mismo.

                break;
            }
        case GizmoType::Rotation:
            {
                // Extraemos valores de tu vieja m_renderConfig
                const float ringRadius = m_gizmoConfig.StickHeight + m_gizmoConfig.ArrowheadHeight; 
                const float ringThickness = m_gizmoConfig.RingThickness; 
                const int ringSegments = m_gizmoConfig.RingTessellation; // ej: 32 o 64
                const int slices = 16; // Resolución del tubo

                // 1. Calcular dirección hacia la cámara en Espacio Local
                Math::Vector3 cameraToModel = m_selectionState.m_gizmoPosition - m_camera.GetPosition();
                cameraToModel.Normalize();
    
                // Invertimos la matriz base para convertir el vector de mundo a local
                Math::Matrix worldInverse = baseMatrix.Invert();
                Math::Vector3 localCamDir = Math::Vector3::TransformNormal(cameraToModel, worldInverse);

                // 2. EJE Y (Anillo Verde - Plano XZ)
                Math::Matrix matY = baseMatrix;
                float angleY = std::atan2f(localCamDir.x, localCamDir.z);
                m_primitiveBatch.DrawHalfTorus(matY, ringRadius, ringThickness, angleY, slices, ringSegments, m_axisColors[1]);

                // 3. EJE X (Anillo Rojo - Plano YZ)
                // Coincide perfecto con tu legacy: Rotamos Z -90 grados
                Math::Matrix matX = Math::Matrix::CreateRotationZ(-Math::PI / 2.0f) * baseMatrix;
                float angleX = std::atan2f(localCamDir.z, localCamDir.y) - (Math::PI * 0.5f);
                m_primitiveBatch.DrawHalfTorus(matX, ringRadius, ringThickness, angleX, slices, ringSegments, m_axisColors[0]);

                // 4. EJE Z (Anillo Azul - Plano XY)
                // Coincide perfecto con tu legacy: Rotamos X 90 grados
                Math::Matrix matZ = Math::Matrix::CreateRotationX(Math::PI / 2.0f) * baseMatrix;
                float angleZ = std::atan2f(localCamDir.y, localCamDir.x) + (Math::PI * 0.5f);
                m_primitiveBatch.DrawHalfTorus(matZ, ringRadius, ringThickness, angleZ, slices, ringSegments, m_axisColors[2]);

                // EXTRA (Opcional): Anillo Exterior Blanco (Rotación dependiente de la cámara)
                // Muchos editores tienen un 4to anillo que siempre mira a la cámara para rotar libremente en pantalla.
                // Si tenías esa funcionalidad o planeas agregarla, se haría con una matriz construida desde GetInverseView().
    
                break;
            }
        }
        
        m_primitiveBatch.End(frameIndex);
    }
    
    void GizmoService::DrawTranslationGizmo(const Math::Matrix& worldTransform, const Math::Vector3& cameraPosition)
    {
        Math::Vector3 origin = worldTransform.Translation();
        float scale = CalculateAdaptiveScale(origin, cameraPosition);

        // Dimensiones proporcionales basadas en la escala adaptativa
        float cylinderLength = 1.8f * scale;
        float cylinderRadius = 0.03f * scale;
        float coneHeight     = 0.4f * scale;
        float coneRadius     = 0.12f * scale;
        int slices           = 16;

        // --- EJE Y (Apantallado por defecto hacia arriba en sistemas Y-Up) ---
        // El cilindro base crece desde el origen en +Y de forma natural.
        Math::Matrix matY = Math::Matrix::CreateTranslation(origin);
        m_primitiveBatch.DrawCylinder(matY, cylinderLength, cylinderRadius, slices, m_axisColors[1]);
        // El cono de la punta se desfasa hasta el final del cilindro
        Math::Matrix matConeY = Math::Matrix::CreateTranslation(0.0f, cylinderLength, 0.0f) * matY;
        m_primitiveBatch.DrawCone(matConeY, coneHeight, coneRadius, slices, m_axisColors[1]);

        // --- EJE X (Rotamos 90 grados en Z para acostar el eje Y local sobre el eje X global) ---
        Math::Matrix matX = Math::Matrix::CreateRotationZ(-Math::PI / 2.0f) * Math::Matrix::CreateTranslation(origin);
        m_primitiveBatch.DrawCylinder(matX, cylinderLength, cylinderRadius, slices, m_axisColors[0]);
        Math::Matrix matConeX = Math::Matrix::CreateTranslation(cylinderLength, 0.0f, 0.0f) * Math::Matrix::CreateTranslation(origin);
        // Para el cono de X, rotamos el cono base para que apunte hacia +X
        Math::Matrix matConeRotX = Math::Matrix::CreateRotationZ(-Math::PI / 2.0f) * matConeX;
        m_primitiveBatch.DrawCone(matConeRotX, coneHeight, coneRadius, slices, m_axisColors[0]);

        // --- EJE Z (Rotamos 90 grados en X para acostar el eje Y local sobre el eje Z global) ---
        Math::Matrix matZ = Math::Matrix::CreateRotationX(Math::PI / 2.0f) * Math::Matrix::CreateTranslation(origin);
        m_primitiveBatch.DrawCylinder(matZ, cylinderLength, cylinderRadius, slices, m_axisColors[2]);
        Math::Matrix matConeZ = Math::Matrix::CreateTranslation(0.0f, 0.0f, cylinderLength) * Math::Matrix::CreateTranslation(origin);
        // Rotamos el cono base para que apunte hacia +Z
        Math::Matrix matConeRotZ = Math::Matrix::CreateRotationX(Math::PI / 2.0f) * matConeZ;
        m_primitiveBatch.DrawCone(matConeRotZ, coneHeight, coneRadius, slices, m_axisColors[2]);
    }

    void GizmoService::DrawRotationGizmo(const Math::Matrix& worldTransform, const Math::Vector3& cameraPosition)
    {
        Math::Vector3 origin = worldTransform.Translation();
        float scale = CalculateAdaptiveScale(origin, cameraPosition);

        float radius = 2.0f * scale;
        float thickness = 0.04f * scale;
        int slices = 48; // Más definición para que las curvas se vean suaves
        int segments = 8;

        // Eje X Anillo (Gira sobre el plano YZ, la matriz rota para orientar el Toroide)
        Math::Matrix matRotX = Math::Matrix::CreateRotationZ(Math::PI / 2.0f) * Math::Matrix::CreateTranslation(origin);
        m_primitiveBatch.DrawTorus(matRotX, radius, thickness, slices, segments, m_axisColors[0]);

        // Eje Y Anillo (Gira sobre el plano XZ)
        Math::Matrix matRotY = Math::Matrix::CreateTranslation(origin);
        m_primitiveBatch.DrawTorus(matRotY, radius, thickness, slices, segments, m_axisColors[1]);

        // Eje Z Anillo (Gira sobre el plano XY)
        Math::Matrix matRotZ = Math::Matrix::CreateRotationX(Math::PI / 2.0f) * Math::Matrix::CreateTranslation(origin);
        m_primitiveBatch.DrawTorus(matRotZ, radius, thickness, slices, segments, m_axisColors[2]);
    }

    void GizmoService::DrawScaleGizmo(const Math::Matrix& worldTransform, const Math::Vector3& cameraPosition)
    {
        Math::Vector3 origin = worldTransform.Translation();
        float scale = CalculateAdaptiveScale(origin, cameraPosition);

        float cylinderLength = 1.8f * scale;
        float cylinderRadius = 0.03f * scale;
        float boxSize        = 0.18f * scale; // En vez de cono, usamos cubos en los extremos
        int slices           = 16;

        // --- EJE Y ---
        Math::Matrix matY = Math::Matrix::CreateTranslation(origin);
        m_primitiveBatch.DrawCylinder(matY, cylinderLength, cylinderRadius, slices, m_axisColors[1]);
        Math::Matrix matBoxY = Math::Matrix::CreateTranslation(0.0f, cylinderLength, 0.0f) * matY;
        m_primitiveBatch.DrawBox(matBoxY, boxSize, m_axisColors[1]);

        // --- EJE X ---
        Math::Matrix matX = Math::Matrix::CreateRotationZ(-Math::PI / 2.0f) * Math::Matrix::CreateTranslation(origin);
        m_primitiveBatch.DrawCylinder(matX, cylinderLength, cylinderRadius, slices, m_axisColors[0]);
        Math::Matrix matBoxX = Math::Matrix::CreateTranslation(cylinderLength, 0.0f, 0.0f) * Math::Matrix::CreateTranslation(origin);
        m_primitiveBatch.DrawBox(matBoxX, boxSize, m_axisColors[0]);

        // --- EJE Z ---
        Math::Matrix matZ = Math::Matrix::CreateRotationX(Math::PI / 2.0f) * Math::Matrix::CreateTranslation(origin);
        m_primitiveBatch.DrawCylinder(matZ, cylinderLength, cylinderRadius, slices, m_axisColors[2]);
        Math::Matrix matBoxZ = Math::Matrix::CreateTranslation(0.0f, 0.0f, cylinderLength) * Math::Matrix::CreateTranslation(origin);
        m_primitiveBatch.DrawBox(matBoxZ, boxSize, m_axisColors[2]);
    }

    void GizmoService::DrawLine(const Math::Vector3& start, const Math::Vector3& end, const Math::Color& color)
    {
        m_primitiveBatch.DrawLine(start, end, color);
    }

    void GizmoService::Render(DX::GraphicsContext* context, uint32_t frameIndex, const Math::Matrix& viewProj)
    {
        if (m_primitiveBatch.GetIndexCount() == 0)
        {
            return;
        }
        // Bind Root Signature y PSO
        context->SetRootSignature(m_rootSignature.get()->GetNative());
        context->SetPipelineState(m_psoDepthOff.get()->GetNative()); // Usar DepthOff si quieres que flote sobre todo
    
        // Bind Constantes (Root Constants o Constant Buffer temporal)
        GizmoConstants constants = { viewProj };
        context->SetGraphicsRoot32BitConstants(0, sizeof(GizmoConstants) / 4, &constants, 0);

        // Bind Buffers
        context->SetVertexBuffer(m_primitiveBatch.GetVertexBuffer(frameIndex)->GetView());
        context->SetIndexBuffer(&m_primitiveBatch.GetIndexBuffer(frameIndex)->GetView());

        // DIBUJAR TODO EL BATCH EN 1 SOLO DRAW CALL
        context->DrawIndexedInstanced(m_primitiveBatch.GetIndexCount(), 1, 0, 0, 0);
    }

    bool GizmoService::BeginDrag(const Math::Vector2& mousePosition)
    {
        if (m_currentGizmoType == GizmoType::None || !m_isActive)
        {
            return false;
        }
        
        auto selectedAxis = GetAxis(mousePosition);

        m_currentAxis = selectedAxis;

        BR_CORE_TRACE << "selectedAxis = " << static_cast<int>(selectedAxis) << std::endl;
        if (m_currentAxis == GizmoAxis::None)
        {
            return false;
        }
        m_selectionState.m_initialGizmoPosition = m_selectionState.m_gizmoPosition;
        
        if (m_currentGizmoType == GizmoType::Translation || m_currentGizmoType == GizmoType::Scale)
        {
            SetGizmoHandlePlaneFor(selectedAxis, mousePosition);
            
            // Solo para traslación necesitamos el punto de intersección inicial real en 3D
            Math::Vector3 intersectionPoint;
            if (GetAxisIntersectionPoint(mousePosition, intersectionPoint))
                m_selectionState.m_prevIntersectionPosition = intersectionPoint;
        }
        else if (m_currentGizmoType == GizmoType::Rotation)
        {
            SetGizmoHandlePlaneForRotation(selectedAxis, mousePosition);
        }

        m_selectionState.m_prevMousePosition = mousePosition;

        if (m_currentGizmoType == GizmoType::Rotation)
        {
            auto cameraViewInverse = m_camera.GetInverseView();
            auto forward = cameraViewInverse.Forward();
            forward.Normalize();

            auto up = cameraViewInverse.Up();
            up.Normalize();

            auto right = cameraViewInverse.Right();
            right.Normalize();

            cameraViewInverse = Math::Matrix::Identity;
            cameraViewInverse.Forward(forward);
            cameraViewInverse.Up(up);
            cameraViewInverse.Right(right);
            m_selectionState.m_cameraViewInverseRotation = Math::Quaternion::CreateFromRotationMatrix(cameraViewInverse);
            m_selectionState.m_cameraViewInverseRotationConjugate = m_selectionState.m_cameraViewInverseRotation;
            m_selectionState.m_cameraViewInverseRotationConjugate.Conjugate();
        }

        m_selectionState.m_isDragging = true;
        return true;
    }

    void GizmoService::Drag(const Math::Vector2& mousePosition)
    {
        switch (m_currentGizmoType)
        {
        case GizmoType::Translation:
            {
                Math::Vector3 currentIntersectionPoint;
                if (GetAxisIntersectionPoint(mousePosition, currentIntersectionPoint))
                {
                    // 1. Movimiento total en ESPACIO LOCAL
                    Math::Vector3 totalMovement = currentIntersectionPoint - m_selectionState.m_prevIntersectionPosition;
                    totalMovement = ApplySnapAndPrecisionMode(totalMovement);

                    // 2. Restringimos al eje seleccionado (Sigue estando en Espacio Local, lo cual es perfecto 
                    // porque ConstrainToAxis solo anula componentes, ej: Vector3(0, 0, Z))
                    totalMovement = ConstrainToAxis(totalMovement, m_currentAxis);

                    // 3. TRANSFORMACIÓN CRÍTICA: Convertir el vector direccional Local a Mundo.
                    // TransformNormal aplica solo la rotación (m_rotationMatrix), ignorando la traslación.
                    Math::Vector3 worldMovement = Math::Vector3::TransformNormal(totalMovement, m_selectionState.m_rotationMatrix);

                    // 4. La nueva posición: Mundo + Mundo
                    m_selectionState.m_gizmoPosition = m_selectionState.m_initialGizmoPosition + worldMovement;

                    if (m_dragTranslationCallback)
                    {
                        m_dragTranslationCallback(m_selectionState.m_gizmoPosition); 
                    }
                }
            
                break;
            }
        case GizmoType::Rotation:
            {
                Math::Quaternion rotationDelta = GetRotationDelta(mousePosition);
    
                // Evitamos disparar callbacks y ensuciar (Dirty Flag) la UI 
                // si el delta de rotación es identidad (no se movió el ratón)
                if (rotationDelta != Math::Quaternion::Identity)
                {
                    // TODO: Apply snap (ej: redondear a 15 grados si se presiona CTRL)
        
                    if(m_dragRotationCallback)
                    {
                        m_dragRotationCallback(rotationDelta);
                    }
                }
                break;
            }
        case GizmoType::Scale:
            {
                auto scaleDelta = GetDeltaMovement(mousePosition);
                scaleDelta = ApplySnapAndPrecisionMode(scaleDelta);
                BR_CORE_TRACE << "scaleDelta: " << scaleDelta << std::endl;

                if (m_dragScaleCallback)
                {
                    m_dragScaleCallback(scaleDelta, m_currentAxis == GizmoAxis::XYZ);
                }
                break;
            }
        }

        UpdateLocalState();
    }

    void GizmoService::EndDrag()
    {
        //OnMouseMove(m_selectionState.m_prevMousePosition);

        m_selectionState.m_prevIntersectionPosition = Math::Vector3::Zero;
        m_selectionState.m_intersectionPosition = Math::Vector3::Zero;
        m_selectionState.m_prevMousePosition = Math::Vector2::Zero;

        m_selectionState.m_isDragging = false;
    }

    void GizmoService::SetTransformSpace(TransformSpace space)
    {
        m_transformSpace = space;
        UpdateLocalState();
    }

    void GizmoService::SetGizmoPosition(const Math::Vector3& position)
    {
        m_selectionState.m_gizmoPosition = position;
        UpdateLocalState();
    }

    void GizmoService::SetGizmoWorldMatrix(const Math::Matrix& worldTransform)
    {
        m_selectionState.m_gizmoWorldMatrix = worldTransform;
        UpdateLocalState();
    }

    float GizmoService::CalculateAdaptiveScale(const Math::Vector3& position, const Math::Vector3& cameraPosition) const
    {
        float distance = Math::Vector3::Distance(position, cameraPosition);
        // Multiplicamos por un factor constante para ajustar el tamaño relativo en el viewport (ej: 0.1)
        float scaleFactor = distance * 0.1f;
    
        // Evitamos escalas absurdas o negativas si la cámara se posiciona exactamente sobre el objeto
        return std::max<float>(scaleFactor, 0.001f);
    }

    GizmoService::GizmoAxis GizmoService::GetAxis(const Math::Vector2& mousePosition)
    {
        float closestIntersection = (std::numeric_limits<float>::max)();
        Math::Vector3 currentIntersection;
        auto selectedAxis = GizmoAxis::None;

        // USAMOS SIEMPRE LA MATRIZ ORIENTADA PARA ROTACIÓN
        Math::Matrix gizmoWorldInverse = m_selectionState.m_gizmoObjectOrientedWorld.Invert();
        Math::Ray ray = ConvertMousePositionToRay(mousePosition);
        ray.position = Math::Vector3::Transform(ray.position, gizmoWorldInverse);
        ray.direction = Math::Vector3::TransformNormal(ray.direction, gizmoWorldInverse);
        ray.direction.Normalize();

        if (m_currentGizmoType == GizmoType::Translation || m_currentGizmoType == GizmoType::Scale)
        {
            float intersection = -1.0f;

            if (XAxisBox.Intersects(ray.position, ray.direction, intersection)) {
                if (intersection < closestIntersection)
                {
                    selectedAxis = GizmoAxis::X;
                    closestIntersection = intersection;
                    currentIntersection = ray.position + (ray.direction * intersection);
                }
            }
            if (YAxisBox.Intersects(ray.position, ray.direction, intersection)) {
                if (intersection < closestIntersection)
                {
                    selectedAxis = GizmoAxis::Y;
                    closestIntersection = intersection;
                    currentIntersection = ray.position + (ray.direction * intersection);
                }
            }
            if (ZAxisBox.Intersects(ray.position, ray.direction, intersection)) {
                if (intersection < closestIntersection)
                {
                    selectedAxis = GizmoAxis::Z;
                    closestIntersection = intersection;
                    currentIntersection = ray.position + (ray.direction * intersection);
                }
            }
            if (m_currentGizmoType == GizmoType::Translation)
            {
                if (closestIntersection >= (std::numeric_limits<float>::max)())
                    closestIntersection = (std::numeric_limits<float>::min)();

                if (XYAxisBox.Intersects(ray.position, ray.direction, intersection)) {
                    if (intersection > closestIntersection)
                    {
                        selectedAxis = GizmoAxis::XY;
                        closestIntersection = intersection;
                        currentIntersection = ray.position + (ray.direction * intersection);
                    }
                }
                if (XZAxisBox.Intersects(ray.position, ray.direction, intersection)) {
                    if (intersection > closestIntersection)
                    {
                        selectedAxis = GizmoAxis::XZ;
                        closestIntersection = intersection;
                        currentIntersection = ray.position + (ray.direction * intersection);
                    }
                }
                if (YZAxisBox.Intersects(ray.position, ray.direction, intersection)) {
                    if (intersection > closestIntersection)
                    {
                        selectedAxis = GizmoAxis::YZ;
                        closestIntersection = intersection;
                        currentIntersection = ray.position + (ray.direction * intersection);
                    }
                }
            }
            else if (m_currentGizmoType == GizmoType::Scale)
            {
                if (XYZAxisBox.Intersects(ray.position, ray.direction, intersection)) {
                    if (intersection < closestIntersection)
                    {
                        selectedAxis = GizmoAxis::XYZ;
                        closestIntersection = intersection;
                        currentIntersection = ray.position + (ray.direction * intersection);
                    }
                }
            }
        }
        else if (m_currentGizmoType == GizmoType::Rotation)
        {
            float outerRadius = Gizmo::GIZMO_LENGTH + m_gizmoConfig.RingThickness * 1.5f;
            float innerRadius = Gizmo::GIZMO_LENGTH - m_gizmoConfig.RingThickness * 1.5f;
            float halfThickness = m_gizmoConfig.RingThickness * 1.5f;

            // Precalcular la ecuación cuadrática de la esfera común (A=1 porque el rayo está normalizado)
            float B = 2.0f * ray.position.Dot(ray.direction);
            float C = ray.position.LengthSquared();

            auto IntersectSphere = [](float R, float b, float c, float& t0, float& t1) {
                float discriminant = b * b - 4.0f * (c - R * R);
                if (discriminant < 0.0f) return false;
                float sq = std::sqrt(discriminant);
                t0 = (-b - sq) * 0.5f;
                t1 = (-b + sq) * 0.5f;
                return true;
            };

            float tOut0, tOut1, tIn0, tIn1;
            bool hitOuter = IntersectSphere(outerRadius, B, C, tOut0, tOut1);
            bool hitInner = IntersectSphere(innerRadius, B, C, tIn0, tIn1);

            // Si el rayo ni siquiera toca la esfera exterior que envuelve al gizmo, ignoramos los anillos
            if (hitOuter)
            {
                Math::Vector3 planeNormals[3]{ Math::Vector3::Right, Math::Vector3::Up, Math::Vector3::Forward };

                for (int i = 0; i < 3; i++)
                {
                    Math::Vector3 N = planeNormals[i];
                    float dotDN = ray.direction.Dot(N);
                    float dotON = ray.position.Dot(N);

                    float tSlab0 = -1e30f;
                    float tSlab1 = 1e30f;

                    if (std::abs(dotDN) < 0.00001f) // Edge-on absoluto
                    {
                        if (std::abs(dotON) > halfThickness) continue; // Pasa por fuera del grosor de este anillo
                    }
                    else 
                    {
                        float t1 = (-halfThickness - dotON) / dotDN;
                        float t2 = ( halfThickness - dotON) / dotDN;
                        tSlab0 = (std::min)(t1, t2);
                        tSlab1 = (std::max)(t1, t2);
                    }

                    // Evaluar superposición de intervalos lógicos (OuterSphere INTERSECT Slab - InnerSphere)
                    auto CheckOverlap = [&](float s0, float s1) {
                        float start = (std::max)(s0, tSlab0);
                        float end = (std::min)(s1, tSlab1);
                        if (start <= end && end >= 0.0f) {
                            float hit_t = (start < 0.0f) ? 0.0f : start; // Si la cámara está dentro del anillo
                            if (hit_t < closestIntersection) {
                                closestIntersection = hit_t;
                                selectedAxis = (GizmoAxis)(i + 1);
                            }
                        }
                    };

                    if (hitInner) {
                        // Si golpea la esfera central, el rayo se divide en dos segmentos por evaluar
                        CheckOverlap(tOut0, tIn0);
                        CheckOverlap(tIn1, tOut1);
                    } else {
                        // Pasa por el borde del gizmo sin tocar el agujero central
                        CheckOverlap(tOut0, tOut1);
                    }
                }
            }
            
            
            // 2. Fallback: Si ningún anillo fue tocado, evaluamos la esfera central (Trackball)
            if (selectedAxis == GizmoAxis::None)
            {
                Math::BoundingSphere trackballSphere(Math::Vector3::Zero, innerRadius);
                float sphereDist;
                if (trackballSphere.Intersects(ray.position, ray.direction, sphereDist))
                {
                    closestIntersection=sphereDist;
                    selectedAxis = GizmoAxis::XYZ;
                }
            }
        }

        if (closestIntersection == (std::numeric_limits<float>::max)() || closestIntersection == (std::numeric_limits<float>::min)())
        {
            selectedAxis = GizmoAxis::None;
        }

        return selectedAxis;
    }

    Math::Vector3 GizmoService::ConstrainToAxis(const Math::Vector3& movement, GizmoAxis axis)
    {
        Math::Vector3 constrainedMovement = Math::Vector3::Zero;

        // ¡La magia del Espacio Local! 
        // Como 'movement' ya está en espacio local, los ejes siempre están alineados 
        // perfectamente con X, Y y Z. Solo copiamos las componentes deseadas.

        switch (axis)
        {
        case GizmoAxis::X:
            constrainedMovement.x = movement.x;
            break;
        case GizmoAxis::Y:
            constrainedMovement.y = movement.y;
            break;
        case GizmoAxis::Z:
            constrainedMovement.z = movement.z;
            break;
        case GizmoAxis::XY:
            constrainedMovement.x = movement.x;
            constrainedMovement.y = movement.y;
            break;
        case GizmoAxis::XZ:
            constrainedMovement.x = movement.x;
            constrainedMovement.z = movement.z;
            break;
        case GizmoAxis::YZ:
            constrainedMovement.y = movement.y;
            constrainedMovement.z = movement.z;
            break;
        case GizmoAxis::XYZ:
        default:
            // Movimiento libre
            constrainedMovement = movement;
            break;
        }

        return constrainedMovement;
    }

    Math::Vector3 GizmoService::ApplySnapAndPrecisionMode(Math::Vector3 delta)
    {
        /* 1. Modo Precisión (Asumiendo que tienes un InputSystem global o inyectado)
        // Reduce el movimiento al 10% de su velocidad original
        if (InputSystem::IsKeyDown(KeyCode::Shift)) 
        {
            delta *= 0.1f; 
        }

        // 2. Modo Snap (Se activa por teclado o por un toggle en la UI del Editor)
        bool isSnapActive = InputSystem::IsKeyDown(KeyCode::Ctrl) || m_gizmoConfig.TranslationSnapEnabled;
    
        if (isSnapActive)
        {
            // Obtenemos el tamaño del "salto" de la grilla (ej: 0.5f, 1.0f)
            float snapStep = m_gizmoConfig.TranslationSnapValue; 
        
            if (snapStep > 0.0f) 
            {
                // Usamos std::round para llevar el valor al múltiplo más cercano del paso
                delta.x = std::round(delta.x / snapStep) * snapStep;
                delta.y = std::round(delta.y / snapStep) * snapStep;
                delta.z = std::round(delta.z / snapStep) * snapStep;
            }
        }*/

        return delta;
    }

    Math::Ray GizmoService::ConvertMousePositionToRay(const Math::Vector2& mousePosition)
    {
        Math::Vector3 nearPoint(mousePosition.x, mousePosition.y, 0.0f);
        Math::Vector3 farPoint(mousePosition.x, mousePosition.y, 1.0f);

        nearPoint = m_camera.GetViewport().Unproject(nearPoint,
                                                     m_camera.GetProjection(),
                                                     m_camera.GetView(),
                                                     Math::Matrix::Identity);

        farPoint = m_camera.GetViewport().Unproject(farPoint,
                                                    m_camera.GetProjection(),
                                                    m_camera.GetView(),
                                                    Math::Matrix::Identity);

        Math::Vector3 direction = farPoint - nearPoint;
        direction.Normalize();

        return Math::Ray(nearPoint, direction);
    }

    bool GizmoService::GetAxisIntersectionPoint(const Math::Vector2& mousePosition, Math::Vector3& intersectionPoint)
    {
        // Si no hay eje o es escalado uniforme, no hay intersección 3D
        if (m_currentAxis == GizmoAxis::None || m_currentAxis == GizmoAxis::XYZ) 
        {
            return false; 
        }

        if (m_currentGizmoType == GizmoType::Translation || m_currentGizmoType == GizmoType::Scale)
        {
            auto gizmoWorldInverse = m_selectionState.m_rotationMatrix.Transpose();

            auto ray = ConvertMousePositionToRay(mousePosition);
            ray.position = Math::Vector3::Transform(ray.position, gizmoWorldInverse);
            ray.direction = Math::Vector3::TransformNormal(ray.direction, gizmoWorldInverse);
            ray.direction.Normalize();

            float intersection;
            if (ray.Intersects(m_selectionState.m_currentGizmoPlane, intersection))
            {
                intersectionPoint = ray.position + (ray.direction * intersection);
                return true;
            }
        }
        else
        {
            if (m_currentAxis == GizmoAxis::XYZ)
            {
                return true;
            }

            auto gizmoWorldInverse = m_selectionState.m_gizmoObjectOrientedWorld.Invert();

            auto ray = ConvertMousePositionToRay(mousePosition);
            ray.position = Math::Vector3::Transform(ray.position, gizmoWorldInverse);
            ray.direction = Math::Vector3::TransformNormal(ray.direction, gizmoWorldInverse);
            ray.direction.Normalize();

            Math::Plane plane = m_selectionState.m_currentGizmoPlane;

            float intersection;
            if (ray.Intersects(plane, intersection))
            {
                intersectionPoint = ray.position + (ray.direction * intersection);
                intersectionPoint.Normalize();
                return true;
            }
        }

        return false;
    }

    Math::Quaternion GizmoService::GetRotationDelta(const Math::Vector2& mousePosition)
    {
        Math::Quaternion rotationDelta = Math::Quaternion::Identity;

        if (m_currentAxis == GizmoAxis::XYZ)
        {
            // ==========================================
            // ROTACIÓN LIBRE (Trackball)
            // ==========================================
            auto gizmoScreenPosition = GetScreenPosition(m_selectionState.m_gizmoPosition);
            // Usamos una longitud de referencia (ej. StickHeight)
            auto gizmoScreenPosition2 = GetScreenPosition(m_selectionState.m_gizmoPosition + m_camera.GetView().Right() * m_gizmoConfig.StickHeight);
        
            float length = 4.0f * (gizmoScreenPosition2 - gizmoScreenPosition).Length() / DirectX::XM_PI;
        
            // Prevención de división por cero si la cámara está demasiado lejos o el gizmo es muy pequeño
            if (length < 0.0001f) return rotationDelta;

            Math::Vector2 deltaAngles(1.0f / length);
            Math::Vector2 mouseVelocity(mousePosition.x - m_selectionState.m_prevMousePosition.x, mousePosition.y - m_selectionState.m_prevMousePosition.y);

            auto angles = mouseVelocity * deltaAngles;

            auto localRotationDelta = Math::Quaternion::CreateFromYawPitchRoll(angles.x, 0.0f, 0.0f) * Math::Quaternion::CreateFromYawPitchRoll(0.0f, angles.y, 0.0f);
                                  
            rotationDelta = m_selectionState.m_cameraViewInverseRotationConjugate * localRotationDelta * m_selectionState.m_cameraViewInverseRotation;
        }
        else
        {
            // 1. Usamos SIEMPRE la inversa del mundo orientado actual
            Math::Matrix gizmoWorldInverse = m_selectionState.m_gizmoObjectOrientedWorld.Invert();
            Math::Vector3 N = m_selectionState.m_currentGizmoPlane.Normal();

            // 2. LAMBDA MÁGICA: Convierte un (X,Y) de pantalla a un punto local en el anillo
            auto GetRingIntersection = [&](const Math::Vector2& screenPos, Math::Vector3& outLocalPoint) -> bool 
            {
                Math::Ray worldRay = ConvertMousePositionToRay(screenPos);
                Math::Ray localRay;
                localRay.position = Math::Vector3::Transform(worldRay.position, gizmoWorldInverse);
                localRay.direction = Math::Vector3::TransformNormal(worldRay.direction, gizmoWorldInverse);
                localRay.direction.Normalize();

                Math::Vector3 D = localRay.direction;
                Math::Vector3 O = localRay.position;

                float dotDN = D.Dot(N);
                float dotON = O.Dot(N);
                float radius = Gizmo::GIZMO_LENGTH;

                float a = 1.0f - dotDN * dotDN; 
                float b = 2.0f * (O.Dot(D) - dotON * dotDN);
                float c = (O.LengthSquared() - dotON * dotON) - (radius * radius);

                float t = -1.0f;
                if (std::abs(a) < 0.0001f) 
                {
                    t = -dotON / dotDN;
                }
                else
                {
                    float delta = b * b - 4.0f * a * c;
                    if (delta >= 0.0f)
                    {
                        float t1 = (-b - std::sqrt(delta)) / (2.0f * a);
                        float t2 = (-b + std::sqrt(delta)) / (2.0f * a);
                        t = (t1 >= 0.0f) ? t1 : t2; 
                    }
                    else
                    {
                        t = -b / (2.0f * a); 
                    }
                }

                if (t >= 0.0f)
                {
                    Math::Vector3 P = O + D * t;
                    outLocalPoint = P - N * P.Dot(N); // Aplastar al plano
                    if (outLocalPoint.LengthSquared() > 0.000001f)
                    {
                        outLocalPoint.Normalize();
                        return true;
                    }
                }
                return false;
            };

            // 3. Proyectamos AMBOS ratones (anterior y actual) en el espacio local DE ESTE FRAME
            Math::Vector3 prevLocalPoint, currentLocalPoint;
        
            bool hitPrev = GetRingIntersection(m_selectionState.m_prevMousePosition, prevLocalPoint);
            bool hitCurr = GetRingIntersection(mousePosition, currentLocalPoint);

            // 4. Calcular el delta si ambos puntos son válidos
            if (hitPrev && hitCurr)
            {
                // 1. Ángulo Absoluto
                float dotProduct = Math::Clamp(prevLocalPoint.Dot(currentLocalPoint), -1.0f, 1.0f);
                float deltaAngle = std::acos(dotProduct);

                // 2. Signo de Rotación (+ o -) en espacio local
                Math::Vector3 crossProduct = prevLocalPoint.Cross(currentLocalPoint);
            
                // Si el movimiento del ratón fue en contra de nuestra normal local, invertimos el ángulo
                if (crossProduct.Dot(N) < 0.0f)
                {
                    deltaAngle = -deltaAngle;
                }

                // 3. ¡LA MAGIA! Convertimos el eje Local (N) en un eje Global (World Space)
                // Usamos la misma matriz de rotación orientada del gizmo actual.
                Math::Vector3 worldAxis = Math::Vector3::TransformNormal(N, m_selectionState.m_gizmoObjectOrientedWorld);
                worldAxis.Normalize();

                // 4. Creamos el cuaternión Delta de rotación usando el EJE GLOBAL
                rotationDelta = Math::Quaternion::CreateFromAxisAngle(worldAxis, deltaAngle);
            }
        }

        m_selectionState.m_prevMousePosition = mousePosition;
    
        return rotationDelta;
    }

    Math::Vector3 GizmoService::GetDeltaMovement(const Math::Vector2& mousePosition)
    {
        Math::Vector3 delta = Math::Vector3::Zero;

        // 1. ESCALA / TRASLACIÓN UNIFORME (Movimiento puro en 2D de pantalla)
        if (m_currentAxis == GizmoAxis::XYZ)
        {
            float sensitivity = 0.01f; 
            float deltaScreen = (mousePosition.x - m_selectionState.m_prevMousePosition.x) - 
                                (mousePosition.y - m_selectionState.m_prevMousePosition.y);

            delta = Math::Vector3(deltaScreen * sensitivity);
            m_selectionState.m_prevMousePosition = mousePosition;
            return delta;
        }

        // 2. TRASLACIÓN / ESCALA EN EJES
        Math::Vector3 currentLocalIntersection;
        if (GetAxisIntersectionPoint(mousePosition, currentLocalIntersection))
        {
            Math::Vector3 localDelta = currentLocalIntersection - m_selectionState.m_prevIntersectionPosition;

            if (m_currentAxis == GizmoAxis::X || m_currentAxis == GizmoAxis::XY || m_currentAxis == GizmoAxis::XZ)
            {
                delta.x = localDelta.x;
            }
            
            if (m_currentAxis == GizmoAxis::Y || m_currentAxis == GizmoAxis::XY || m_currentAxis == GizmoAxis::YZ)
            {
                delta.y = localDelta.y;
            }
            
            if (m_currentAxis == GizmoAxis::Z || m_currentAxis == GizmoAxis::XZ || m_currentAxis == GizmoAxis::YZ)
            {
                delta.z = localDelta.z;
            }

            m_selectionState.m_prevIntersectionPosition = currentLocalIntersection;
        }

        return delta;
    }

    float GizmoService::GetCameraDistance() const
    {
        if (m_camera.IsOrthographic()) 
        {
            // En vista ortográfica, la escala en pantalla no depende de la distancia en Z, 
            // sino del tamaño del viewport u orthographic size. Este valor fijo (25.0f) 
            // asume una configuración estática, lo cual es correcto para proyecciones orto.
            return 25.0f; 
        }

        // Transformamos la posición del gizmo al View Space de la cámara
        Math::Vector3 gizmoPositionViewSpace = Math::Vector3::Transform(m_selectionState.m_gizmoPosition, m_camera.GetView());
    
        // Extraer el Z absoluto (distancia en profundidad). 
        // Esto evita que el gizmo se deforme si está en los bordes de la pantalla (fov distortion)
        float depth = Math::Abs(gizmoPositionViewSpace.z);

        // Retornamos la distancia solo si está frente a la cámara (más allá del Near Plane)
        return (depth > m_camera.GetNearPlane()) ? depth : 0.0f;
    }

    void GizmoService::UpdateLocalState()
    {
        // 1. Calcular Escala de Pantalla (Screen Space Scale)
        float cameraDistance = GetCameraDistance();
        m_selectionState.m_screenScaleFactor = (cameraDistance > 0.0f) ? (cameraDistance * Gizmo::GIZMO_SCREEN_SCALE) : 1.0f;
        m_selectionState.m_screenScaleMatrix = Math::Matrix::CreateScale(m_selectionState.m_screenScaleFactor);

        // 2. Obtener la rotación base del objeto seleccionado (o Identidad si es TransformSpace::World)
        Math::Matrix baseRotationMatrix = Math::Matrix::Identity;
    
        // Si estamos en Local Space, usamos la rotación del objeto.
        // (A menos que estemos en Rotation y Dragging, donde a veces queremos mantener el marco inicial)
        if (m_transformSpace == TransformSpace::Local) 
        {
            // Solo necesitamos la rotación, nos aseguramos de no traer traslaciones del objeto
            baseRotationMatrix = m_selectionState.m_gizmoWorldMatrix;
            baseRotationMatrix.Translation(Math::Vector3::Zero); 
        }

        // Extraemos y normalizamos los ejes para asegurar una matriz ortonormal pura
        Math::Vector3 right   = baseRotationMatrix.Right();   right.Normalize();
        Math::Vector3 up      = baseRotationMatrix.Up();      up.Normalize();
        Math::Vector3 forward = baseRotationMatrix.Forward(); forward.Normalize();

        // Guardamos la matriz de rotación pura en el estado
        m_selectionState.m_rotationMatrix = Math::Matrix::Identity;
        m_selectionState.m_rotationMatrix.Right(right);
        m_selectionState.m_rotationMatrix.Up(up);
        m_selectionState.m_rotationMatrix.Forward(forward);

        // 3. Construir la Matriz Orientada al Objeto (OOBB)
        // Orden de multiplicación DX12: Escala * Rotación * Traslación
        Math::Matrix gizmoWorld = m_selectionState.m_rotationMatrix * Math::Matrix::CreateTranslation(m_selectionState.m_gizmoPosition);
        m_selectionState.m_gizmoObjectOrientedWorld = m_selectionState.m_screenScaleMatrix * gizmoWorld;

        // 4. Construir la Matriz Alineada a los Ejes Globales (AABB)
        // Para World Space, la rotación es la identidad (excepto quizás remapear el Z, dependiendo de tu sistema de coordenadas)
        Math::Matrix axisAlignedRotation = Math::Matrix::CreateWorld(Math::Vector3::Zero, Math::Vector3::Backward, Math::Vector3::Up);
        Math::Matrix gizmoAxisAlignedWorld = axisAlignedRotation * Math::Matrix::CreateTranslation(m_selectionState.m_gizmoPosition);
    
        // En el modo de escala, las escalas SIEMPRE deben ser locales, los motores AAA no permiten 
        // escalar en World Space porque se deforma (shear) la malla.
        if (m_currentGizmoType == GizmoType::Scale)
        {
            m_selectionState.m_gizmoAxisAlignedWorld = m_selectionState.m_gizmoObjectOrientedWorld;
        }
        else
        {
            m_selectionState.m_gizmoAxisAlignedWorld = m_selectionState.m_screenScaleMatrix * gizmoAxisAlignedWorld;
        }
    }

    Math::Vector2 GizmoService::GetScreenPosition(const Math::Vector3& worldPosition)
    {
        auto point = m_camera.GetViewport().Project(worldPosition,
                                                    m_camera.GetProjection(),
                                                    m_camera.GetView(),
                                                    Math::Matrix::Identity);

        return Math::Vector2(point.x, point.y);
    }

    void GizmoService::SetGizmoHandlePlaneFor(GizmoAxis selectedAxis, const Math::Vector2& mousePosition)
    {
        auto ray = ConvertMousePositionToRay(mousePosition);
        auto toLocal = m_selectionState.m_rotationMatrix.Transpose();

        ray.position = Math::Vector3::Transform(ray.position, toLocal);
        ray.direction = Math::Vector3::TransformNormal(ray.direction, toLocal);
        ray.direction.Normalize();

        SetGizmoHandlePlaneFor(selectedAxis, ray);
    }

    void GizmoService::SetGizmoHandlePlaneForRotation(GizmoAxis selectedAxis, const Math::Vector2& mousePosition)
    {
        if (selectedAxis == GizmoAxis::XYZ)
        {
            return;
        }

        // Al transformar el rayo al espacio del Gizmo en GetRotationDelta, 
        // las normales SIEMPRE son los ejes puros locales.
        Math::Vector3 planeNormals[3]{ Math::Vector3::Right, Math::Vector3::Up, Math::Vector3::Forward };
        int planeIndex = static_cast<int>(selectedAxis) - 1;
    
        // Distancia 0, porque el Gizmo está en el (0,0,0) del espacio local
        m_selectionState.m_currentGizmoPlane = Math::Plane(planeNormals[planeIndex], 0.0f);
    }

    void GizmoService::SetGizmoHandlePlaneFor(GizmoAxis selectedAxis, const Math::Ray& ray)
    {
        auto toLocal = m_selectionState.m_rotationMatrix.Transpose();

        Math::Vector3 gizmoPositionInLocal = Math::Vector3::Transform(m_selectionState.m_gizmoPosition, toLocal);
        Math::Vector3 planeNormal;
        float planeD = 0.0f;

        switch (selectedAxis)
        {
        case GizmoAxis::XY:
            planeNormal = Math::Vector3::Backward;
            planeD = gizmoPositionInLocal.z;
            break;
        case GizmoAxis::YZ:
            planeNormal = Math::Vector3::Left;
            planeD = gizmoPositionInLocal.x;
            break;
        case GizmoAxis::XZ:
            planeNormal = Math::Vector3::Down;
            planeD = gizmoPositionInLocal.y;
            break;

        case GizmoAxis::X:
        case GizmoAxis::Y:
        case GizmoAxis::Z:
            {
                auto cameraToGizmo = m_selectionState.m_gizmoPosition - m_camera.GetPosition();
                cameraToGizmo.Normalize();
                cameraToGizmo = Math::Vector3::TransformNormal(cameraToGizmo, toLocal);

                int axisIndex = static_cast<int>(selectedAxis) - 1;

                Math::Vector3 perpendicularRayVector;
                m_unaryDirections[axisIndex].Cross(cameraToGizmo, perpendicularRayVector);
            
                perpendicularRayVector = m_unaryDirections[axisIndex].Cross(perpendicularRayVector);
            
                float newD = -perpendicularRayVector.Dot(gizmoPositionInLocal);

                perpendicularRayVector.Normalize();
                planeNormal = perpendicularRayVector;
                planeD = newD;
            }
            break;

        case GizmoAxis::XYZ:
            {
                auto cameraToGizmo = m_camera.GetPosition() - m_selectionState.m_gizmoPosition;
                cameraToGizmo = Math::Vector3::TransformNormal(cameraToGizmo, toLocal);

                float zCamera = cameraToGizmo.Length();
                cameraToGizmo.Normalize();

                planeNormal = cameraToGizmo;
                planeD = zCamera;
            }
            break;
        }

        m_selectionState.m_currentGizmoPlane = Math::Plane(planeNormal, planeD);
        BR_CORE_TRACE << "selected plane: " << m_selectionState.m_currentGizmoPlane << std::endl;
    }
}
