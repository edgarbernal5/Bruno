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
    GizmoService::GizmoService(GraphicsDevice* device, Camera& camera) : m_device(device), m_camera(camera)
    {
        for (size_t i = 0; i < 3; i++)
        {
            //m_activeAxisColors[i] = m_axisColors[i];
        }
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
        m_primitiveBatch.Begin();
        
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

    void GizmoService::Render(DX::GraphicsContext* context, const Math::Matrix& viewProj)
    {
        if (m_primitiveBatch.GetIndexCount() == 0) return;

        // Bind Root Signature y PSO
        context->SetRootSignature(m_rootSignature.get()->GetNative());
        context->SetPipelineState(m_psoDepthOff.get()->GetNative()); // Usar DepthOff si quieres que flote sobre todo
    
        // Bind Constantes (Root Constants o Constant Buffer temporal)
        GizmoConstants constants = { viewProj };
        context->SetGraphicsRoot32BitConstants(0, sizeof(GizmoConstants) / 4, &constants, 0);

        // Bind Buffers
        context->SetVertexBuffer(m_primitiveBatch.GetVertexBuffer()->GetView());
        context->SetIndexBuffer(&m_primitiveBatch.GetIndexBuffer()->GetView());

        // DIBUJAR TODO EL BATCH EN 1 SOLO DRAW CALL
        context->DrawIndexedInstanced(m_primitiveBatch.GetIndexCount(), 1, 0, 0, 0);
    
        // Limpiamos los arrays de CPU para el siguiente frame
        m_primitiveBatch.Begin(); 
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
            Math::Vector3 planeNormals[3]{ Math::Vector3::Right, Math::Vector3::Up, Math::Vector3::Forward };

            auto currentPointOnPlane = Math::Vector3::Zero;
            for (int i = 0; i < 3; i++)
            {
                auto plane = Math::Plane(planeNormals[i], 0);
                float intersection;

                if (ray.Intersects(plane, intersection))
                {
                    auto positionOnPlane = ray.position + (ray.direction * intersection);
                    if (positionOnPlane.Length() > (Gizmo::GIZMO_LENGTH + m_gizmoConfig.RingThickness * 1.5f))
                    {
                        continue;
                    }
                    else
                    {
                        if (intersection < closestIntersection)
                        {
                            currentPointOnPlane = positionOnPlane;
                            closestIntersection = intersection;

                            selectedAxis = (GizmoAxis)(i + 1);
                        }
                    }
                }
            }
            if (selectedAxis != GizmoAxis::None)
            {
                if (currentPointOnPlane.Length() < (Gizmo::GIZMO_LENGTH - m_gizmoConfig.RingThickness * 1.5f))
                {
                    selectedAxis = GizmoAxis::XYZ;
                }
            }
        }

        if (closestIntersection >= (std::numeric_limits<float>::max)() || closestIntersection <= (std::numeric_limits<float>::min)())
        {
            selectedAxis = GizmoAxis::None;
        }

        return selectedAxis;
    }

    Math::Vector3 GizmoService::ConstrainToAxis(const Math::Vector3& movement, GizmoAxis axis)
    {
        Math::Vector3 constrainedMovement = Math::Vector3::Zero;
    
        // Extraemos los ejes direccionales normalizados de la rotación actual del gizmo
        // Asumiendo que m_gizmoObjectOrientedWorld es la matriz de transformación de la herramienta
        Math::Vector3 right = m_selectionState.m_gizmoObjectOrientedWorld.Right();
        Math::Vector3 up = m_selectionState.m_gizmoObjectOrientedWorld.Up();
        Math::Vector3 forward = m_selectionState.m_gizmoObjectOrientedWorld.Forward();

        right.Normalize(); 
        up.Normalize(); 
        forward.Normalize();

        switch (axis)
        {
        case GizmoAxis::X:
            // Proyección sobre el vector Right (X)
            constrainedMovement = right * movement.Dot(right);
            break;
        case GizmoAxis::Y:
            // Proyección sobre el vector Up (Y)
            constrainedMovement = up * movement.Dot(up);
            break;
        case GizmoAxis::Z:
            // Proyección sobre el vector Forward (Z)
            constrainedMovement = forward * movement.Dot(forward);
            break;
        case GizmoAxis::XY:
            // Si nos movemos en el plano XY, eliminamos cualquier movimiento en el eje Z (Forward)
            constrainedMovement = movement - forward * movement.Dot(forward);
            break;
        case GizmoAxis::XZ:
            // Eliminamos el movimiento en el eje Y (Up)
            constrainedMovement = movement - up * movement.Dot(up);
            break;
        case GizmoAxis::YZ:
            // Eliminamos el movimiento en el eje X (Right)
            constrainedMovement = movement - right * movement.Dot(right);
            break;
        case GizmoAxis::XYZ:
        default:
            // Movimiento libre en todos los ejes simultáneamente
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
        intersectionPoint = Math::Vector3::Zero;
        if (m_currentGizmoType == GizmoType::None || !m_isActive)
            return false;

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

            auto gizmoWorldInverse = m_selectionState.m_gizmoAxisAlignedWorld.Invert();

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
        //if (m_selectionService->GetSelections().empty())
        {
            //return;
        }

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
            //baseRotationMatrix = m_selectionService->GetSelectionTransform();
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

    bool GizmoService::BeginDrag(const Math::Vector2& mousePosition)
    {
        if (m_currentGizmoType == GizmoType::None || !m_isActive)
        {
            return false;
        }
        
        auto selectedAxis = GetAxis(mousePosition);

        m_currentAxis = selectedAxis;

        BR_CORE_TRACE << "selectedAxis = " << (int)selectedAxis << std::endl;
        if (m_currentAxis == GizmoAxis::None)
        {
            return false;
        }
        m_selectionState.m_initialGizmoPosition = m_selectionState.m_gizmoPosition;
        if (m_currentGizmoType == GizmoType::Translation || m_currentGizmoType == GizmoType::Scale)
        {
            SetGizmoHandlePlaneFor(selectedAxis, mousePosition);
        }
        else if (m_currentGizmoType == GizmoType::Rotation)
        {
            SetGizmoHandlePlaneForRotation(selectedAxis, mousePosition);
        }

        Math::Vector3 intersectionPoint;
        if (GetAxisIntersectionPoint(mousePosition, intersectionPoint))
        {
            m_selectionState.m_prevMousePosition = mousePosition;
            m_selectionState.m_prevIntersectionPosition = intersectionPoint;
        }
        else
        {
            BR_CORE_TRACE << "No intersection point found! plane: " << m_selectionState.m_currentGizmoPlane << std::endl;

            m_selectionState.m_isDragging = false;
            m_currentAxis = GizmoAxis::None;
            return false;
        }

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
                    // Calculamos el movimiento TOTAL desde que se hizo clic
                    Math::Vector3 totalMovement = currentIntersectionPoint - m_selectionState.m_prevIntersectionPosition;
                    totalMovement = ApplySnapAndPrecisionMode(totalMovement);
    
                    // Proyectamos el movimiento sobre el eje seleccionado (si no es XYZ)
                    totalMovement = ConstrainToAxis(totalMovement, m_currentAxis);

                    // La nueva posición es el ancla inicial + movimiento total (Cero pérdida de precisión)
                    m_selectionState.m_gizmoPosition = m_selectionState.m_initialGizmoPosition + totalMovement;

                    if (m_dragTranslationCallback)
                    {
                        // Puedes pasar la posición absoluta, o el delta absoluto si el cliente lo requiere
                        m_dragTranslationCallback(m_selectionState.m_gizmoPosition); 
                    }
                }
            
                break;
            }
        case GizmoType::Rotation:
            {
                /*auto rotationDelta = GetRotationDelta(mousePosition);
                //TODO: Apply snap and precision mode
            
                if(m_dragRotationCallback)
                    m_dragRotationCallback(rotationDelta);
*/
                break;
            }
        case GizmoType::Scale:
            {
                /*auto scaleDelta = GetDeltaMovement(mousePosition);
                scaleDelta = ApplySnapAndPrecisionMode(scaleDelta);
                BR_CORE_TRACE << "scaleDelta: " << scaleDelta << std::endl;

                if (m_dragScaleCallback)
                    m_dragScaleCallback(scaleDelta, m_currentAxis == GizmoAxis::XYZ);
*/
                break;
            }
        }

        UpdateLocalState();
        //BuildGeometry();
    }

    void GizmoService::EndDrag()
    {
        //OnMouseMove(m_selectionState.m_prevMousePosition);

        m_selectionState.m_prevIntersectionPosition = Math::Vector3::Zero;
        m_selectionState.m_intersectionPosition = Math::Vector3::Zero;
        m_selectionState.m_prevMousePosition = Math::Vector2::Zero;

        m_selectionState.m_isDragging = false;
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
            return;

        Math::Vector3 planeNormals[3]{ Math::Vector3::Right, Math::Vector3::Up, Math::Vector3::Forward };
        if (m_transformSpace == TransformSpace::Local)
        {
            /*auto localObjectRotationMatrix = m_selectionService->GetSelectionTransform();
            auto localForward = localObjectRotationMatrix.Forward();
            localForward.Normalize();

            auto localUp = localObjectRotationMatrix.Up();
            localUp.Normalize();

            auto localRight = localObjectRotationMatrix.Right();
            localRight.Normalize();

            planeNormals[0] = localRight;
            planeNormals[1] = localUp;
            planeNormals[2] = localForward;*/
        }
        int planeIndex = (int)selectedAxis - 1;

        m_selectionState.m_currentGizmoPlane = Math::Plane(planeNormals[planeIndex], 0);
    }

    void GizmoService::SetGizmoHandlePlaneFor(GizmoAxis selectedAxis, const Math::Ray& ray)
    {
        auto toLocal = m_selectionState.m_rotationMatrix.Transpose();

        Math::Vector3 gizmoPositionInLocal = Math::Vector3::Transform(m_selectionState.m_gizmoPosition, toLocal);
        Math::Plane plane;
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

            int axisIndex = (int)selectedAxis - 1;

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

    void GizmoService::BuildGeometry()
    {
        m_primitiveBatch.Begin();

        if (m_currentGizmoType == GizmoType::None || !m_isActive)
        {
            m_primitiveBatch.End(m_device);
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
        
        m_primitiveBatch.End(m_device);
    }
}
