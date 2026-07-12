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
        m_selectionState.m_gizmoWorld = Math::Matrix::Identity;
        m_selectionState.m_rotationMatrix = Math::Matrix::Identity;
        m_selectionState.m_gizmoObjectOrientedWorld = Math::Matrix::Identity;
        m_selectionState.m_gizmoAxisAlignedWorld = Math::Matrix::Identity;
        m_selectionState.m_screenScaleMatrix = Math::Matrix::Identity;
        m_selectionState.m_gizmoPosition = Math::Vector3::Zero;
        m_selectionState.m_isDragging = false;
        //m_translationScaleSnapDelta = Math::Vector3::Zero;

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
        D3D12_DEPTH_STENCIL_DESC depthDesc = {};
        psoDesc.DepthStencilState.DepthEnable = TRUE;
        psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO; // No escriben en el Z-Buffer
        psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;    // O la que uses en tu motor
        psoDesc.DepthStencilState = depthDesc;
        
        D3D12_RENDER_TARGET_BLEND_DESC blendDesc = {};
        blendDesc.BlendEnable = TRUE;
        blendDesc.LogicOpEnable = FALSE;
        // El color del anillo se multiplica por su propio Alpha (0.15)
        blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA; 
        // El color del fondo se multiplica por (1.0 - 0.15 = 0.85)
        blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA; 
        blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
        // El canal alfa en sí (opcional dependiendo de si compones a otra textura)
        blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
        blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
        blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
        blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

        psoDesc.BlendState.RenderTarget[0] = blendDesc;
        
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
        BuildGizmoGeometry(frameIndex);
    }

    void GizmoService::BuildGizmoGeometry(uint32_t frameIndex)
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

        // ==========================================
        // 1. CONFIGURACIONES COMUNES
        // ==========================================
        const float stickHeight   = m_gizmoConfig.StickHeight;
        const float lineOffset    = m_gizmoConfig.LineOffset;
        const float stickRadius   = m_gizmoConfig.StickRadius;
        const int   slices        = m_gizmoConfig.Tessellation;
        const bool  isDragging    = m_selectionState.m_isDragging;

        // ==========================================
        // 2. LÓGICA CENTRAL DE COLORES (Hover & Fade)
        // ==========================================
        // Una sola lambda resuelve todas las combinaciones de estado interactivo
        auto resolveColor = [&](GizmoAxis axis, Math::Color baseColor, float normalAlpha, float hoverAlpha, bool applyFade) 
        {
            // 1. Desvanecer ejes inactivos durante el arrastre (Usado en Rotación y Escala)
            if (applyFade && isDragging && m_currentAxis != axis)
                return Math::Color(baseColor.R(), baseColor.G(), baseColor.B(), 0.15f);
        
            // 2. Resaltar en amarillo (Hover/Active) si el cursor está sobre este elemento
            if (m_currentAxis == axis)
                return Math::Color(1.0f, 1.0f, 0.0f, hoverAlpha);
            
            // 3. Color y opacidad por defecto (Reposo)
            return Math::Color(baseColor.R(), baseColor.G(), baseColor.B(), normalAlpha);
        };

        // ==========================================
        // 3. DEFINICIÓN DE EJES PRINCIPALES (Data-Driven)
        // ==========================================
        struct AxisDef {
            GizmoAxis axis;
            Math::Color color;
            Math::Matrix localRot;
        };

        // Esto elimina toda la repetición matemática de Y, X, Z.
        const AxisDef axes[3] = {
            { GizmoAxis::X, m_axisColors[0], Math::Matrix::CreateRotationZ(-Math::PI / 2.0f) },
            { GizmoAxis::Y, m_axisColors[1], Math::Matrix::Identity },
            { GizmoAxis::Z, m_axisColors[2], Math::Matrix::CreateRotationX(Math::PI / 2.0f) }
        };

        // ==========================================
        // 4. GENERACIÓN DE GEOMETRÍA
        // ==========================================
        switch (m_currentGizmoType)
        {
        case GizmoType::Translation:
            {
                const float arrowHeight  = m_gizmoConfig.ArrowheadHeight;
                const float arrowRadius  = m_gizmoConfig.ArrowheadRadius;
                const float cylLength    = stickHeight - lineOffset;
                const float cylCenterY   = lineOffset + (cylLength * 0.5f);
            
                // A. Ejes 3D Lineales (X, Y, Z)
                for (const auto& def : axes)
                {
                    Math::Matrix mat = def.localRot * baseMatrix;
                    Math::Color col = resolveColor(def.axis, def.color, 1.0f, 1.0f, false);
                
                    m_primitiveBatch.DrawCylinder(Math::Matrix::CreateTranslation(0.0f, cylCenterY, 0.0f) * mat, cylLength, stickRadius, slices, col);
                    m_primitiveBatch.DrawCone(Math::Matrix::CreateTranslation(0.0f, stickHeight, 0.0f) * mat, arrowHeight, arrowRadius, slices, col);
                }

                // B. Planos 2D de Traslación (XY, XZ, YZ)
                struct PlaneDef { GizmoAxis axis; Math::Color color; Math::Vector3 trans; Math::Vector3 scale; };
            
                const float pSize = lineOffset;
                const float pOff = pSize * 0.5f;
                const PlaneDef planes[3] = {
                    { GizmoAxis::XY, m_axisColors[2], {pOff, pOff, 0.0f}, {pSize, pSize, 0.005f} },
                    { GizmoAxis::XZ, m_axisColors[1], {pOff, 0.0f, pOff}, {pSize, 0.005f, pSize} },
                    { GizmoAxis::YZ, m_axisColors[0], {0.0f, pOff, pOff}, {0.005f, pSize, pSize} }
                };

                for (const auto& p : planes)
                {
                    Math::Matrix mat = Math::Matrix::CreateScale(p.scale) * Math::Matrix::CreateTranslation(p.trans) * baseMatrix;
                    Math::Color col = resolveColor(p.axis, p.color, 0.25f, 0.7f, false);
                    m_primitiveBatch.DrawBox(mat, 1.0f, col);
                }

                // C. Centro (Escala Global)
                Math::Color centerCol = resolveColor(GizmoAxis::XYZ, Math::Color(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 1.0f, false);
                m_primitiveBatch.DrawBox(baseMatrix, lineOffset * 0.3f, centerCol);
                break;
            }

        case GizmoType::Scale:
            {
                const float boxSize      = m_gizmoConfig.ArrowheadHeight * 0.85f;
                const float cylLength    = stickHeight - lineOffset;
                const float cylCenterY   = lineOffset + (cylLength * 0.5f);

                // A. Ejes 3D de Escala (X, Y, Z)
                for (const auto& def : axes)
                {
                    Math::Matrix mat = def.localRot * baseMatrix;
                    Math::Color col = resolveColor(def.axis, def.color, 1.0f, 0.7f, true); // Aplica desvanecimiento al arrastrar
                
                    m_primitiveBatch.DrawCylinder(Math::Matrix::CreateTranslation(0.0f, cylCenterY, 0.0f) * mat, cylLength, stickRadius, slices, col);
                    m_primitiveBatch.DrawBox(Math::Matrix::CreateTranslation(0.0f, stickHeight, 0.0f) * mat, boxSize, col);
                }

                // B. Centro (Escala Uniforme XYZ)
                Math::Color centerCol = resolveColor(GizmoAxis::XYZ, Math::Color(0.8f, 0.8f, 0.8f, 1.0f), 0.7f, 0.7f, false);
                m_primitiveBatch.DrawBox(baseMatrix, lineOffset * 0.75f, centerCol);
                break;
            }

        case GizmoType::Rotation:
            {
                const float ringRadius    = stickHeight + m_gizmoConfig.ArrowheadHeight; 
                const float ringThickness = m_gizmoConfig.RingThickness; 
                const int   ringSegments  = m_gizmoConfig.RingTessellation; 
                const int   torusSlices   = 16; 
            
                // A. Esfera Trackball Central
                Math::Matrix sphereMat = Math::Matrix::CreateTranslation(m_selectionState.m_gizmoPosition);
                float sphereAlpha = (m_currentAxis == GizmoAxis::XYZ) ? 0.3f : 0.05f; 
                m_primitiveBatch.DrawSphere(sphereMat, ringRadius * 0.95f, torusSlices, ringSegments, Math::Color(1.0f, 1.0f, 1.0f, sphereAlpha));

                // B. Anillos 3D (X, Y, Z)
                Math::Matrix dragBaseMat = isDragging ? m_selectionState.m_initialGizmoWorld : baseMatrix;
                Math::Vector3 camPosWorld = m_camera.GetPosition();

                for (const auto& def : axes)
                {
                    Math::Matrix axisBaseMat = (isDragging && m_currentAxis == def.axis) ? dragBaseMat : baseMatrix;
                    Math::Matrix mat = def.localRot * axisBaseMat;
                    Math::Color col = resolveColor(def.axis, def.color, 1.0f, 0.7f, true); // Aplica desvanecimiento al arrastrar
                
                    Math::Vector3 camPosLocal = Math::Vector3::Transform(camPosWorld, mat.Invert());
                    float angle = std::atan2(camPosLocal.z, camPosLocal.x) - (Math::PI * 0.5f);
                
                    m_primitiveBatch.DrawHalfTorus(mat, ringRadius, ringThickness, angle, torusSlices, ringSegments, col);
                }
                break;
            }
        }
    
        m_primitiveBatch.End(frameIndex);
    }

    void GizmoService::RenderBatch(DX::GraphicsContext* context, DX::PrimitiveBatch& primitiveBatch, uint32_t frameIndex, const Math::Matrix& viewProjection)
    {
        if (primitiveBatch.GetIndexCount() == 0)
        {
            return;
        }
        
        // Bind Root Signature y PSO
        context->SetRootSignature(m_rootSignature.get()->GetNative());
        context->SetPipelineState(m_psoDepthOff.get()->GetNative()); // Usar DepthOff si quieres que flote sobre todo
    
        // Bind Constantes (Root Constants o Constant Buffer temporal)
        GizmoConstants constants = { viewProjection };
        context->SetGraphicsRoot32BitConstants(0, sizeof(GizmoConstants) / 4, &constants, 0);

        // Bind Buffers
        context->SetVertexBuffer(primitiveBatch.GetVertexBuffer(frameIndex)->GetView());
        context->SetIndexBuffer(&primitiveBatch.GetIndexBuffer(frameIndex)->GetView());

        // DIBUJAR TODO EL BATCH EN 1 SOLO DRAW CALL
        context->DrawIndexedInstanced(primitiveBatch.GetIndexCount(), 1, 0, 0, 0);
    }

    void GizmoService::Render(DX::GraphicsContext* context, uint32_t frameIndex, const Math::Matrix& viewProjection)
    {
        //Math::Viewport viewport=m_camera.GetViewport();
        //RenderCameraGizmo(context, frameIndex, viewport);
        
        RenderBatch(context, m_primitiveBatch, frameIndex, viewProjection);
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
        m_selectionState.m_initialGizmoWorld = (m_transformSpace == TransformSpace::Local || m_currentGizmoType == GizmoType::Scale) 
                                      ? m_selectionState.m_gizmoObjectOrientedWorld 
                                      : m_selectionState.m_gizmoAxisAlignedWorld;
        
        if (m_currentGizmoType == GizmoType::Translation || m_currentGizmoType == GizmoType::Scale)
        {
            SetGizmoHandlePlaneFor(selectedAxis, mousePosition);
            
            // Solo para traslación necesitamos el punto de intersección inicial real en 3D
            Math::Vector3 intersectionPoint;
            if (GetAxisIntersectionPoint(mousePosition, intersectionPoint))
            {
                m_selectionState.m_prevIntersectionPosition = intersectionPoint;
            }
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
        m_currentAxis = GizmoAxis::None;
    }

    void GizmoService::Update()
    {
        UpdateLocalState();
    }

    void GizmoService::OnMouseMove(const Math::Vector2& mousePosition)
    {
        if (!m_isActive || m_selectionState.m_isDragging)
        {
            return;
        }
        m_currentAxis = GetAxis(mousePosition);
    }

    void GizmoService::SetGizmoType(GizmoType type)
    {
        m_currentGizmoType = type;
        UpdateLocalState();
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
        m_selectionState.m_gizmoWorld = worldTransform;
        UpdateLocalState();
    }
    

    GizmoAxis GizmoService::GetAxis(const Math::Vector2& mousePosition)
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

            if (XAxisBox.Intersects(ray.position, ray.direction, intersection))
            {
                if (intersection < closestIntersection)
                {
                    selectedAxis = GizmoAxis::X;
                    closestIntersection = intersection;
                    currentIntersection = ray.position + (ray.direction * intersection);
                }
            }
            if (YAxisBox.Intersects(ray.position, ray.direction, intersection))
            {
                if (intersection < closestIntersection)
                {
                    selectedAxis = GizmoAxis::Y;
                    closestIntersection = intersection;
                    currentIntersection = ray.position + (ray.direction * intersection);
                }
            }
            if (ZAxisBox.Intersects(ray.position, ray.direction, intersection))
            {
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

                if (XYAxisBox.Intersects(ray.position, ray.direction, intersection))
                {
                    if (intersection > closestIntersection)
                    {
                        selectedAxis = GizmoAxis::XY;
                        closestIntersection = intersection;
                        currentIntersection = ray.position + (ray.direction * intersection);
                    }
                }
                if (XZAxisBox.Intersects(ray.position, ray.direction, intersection))
                {
                    if (intersection > closestIntersection)
                    {
                        selectedAxis = GizmoAxis::XZ;
                        closestIntersection = intersection;
                        currentIntersection = ray.position + (ray.direction * intersection);
                    }
                }
                if (YZAxisBox.Intersects(ray.position, ray.direction, intersection))
                {
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
                if (XYZAxisBox.Intersects(ray.position, ray.direction, intersection))
                {
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

            auto IntersectSphere = [](float R, float b, float c, float& t0, float& t1)
            {
                float discriminant = b * b - 4.0f * (c - R * R);
                if (discriminant < 0.0f)
                {
                    return false;
                }
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
                        if (std::abs(dotON) > halfThickness)
                            continue; // Pasa por fuera del grosor de este anillo
                    }
                    else 
                    {
                        float t1 = (-halfThickness - dotON) / dotDN;
                        float t2 = ( halfThickness - dotON) / dotDN;
                        tSlab0 = (std::min)(t1, t2);
                        tSlab1 = (std::max)(t1, t2);
                    }

                    // Evaluar superposición de intervalos lógicos (OuterSphere INTERSECT Slab - InnerSphere)
                    // Evaluar superposición de intervalos lógicos (OuterSphere INTERSECT Slab - InnerSphere)
                    auto CheckOverlap = [&](float s0, float s1)
                    {
                        float start = (std::max)(s0, tSlab0);
                        float end = (std::min)(s1, tSlab1);
                        if (start <= end && end >= 0.0f)
                        {
                            float hit_t = (start < 0.0f) ? 0.0f : start; 
        
                            // ¡LA MAGIA! Calculamos el punto exacto de colisión en el espacio local
                            Math::Vector3 hitPoint = ray.position + ray.direction * hit_t;
        
                            // Si el producto punto es mayor a 0, el punto está en el hemisferio que mira a la cámara
                            if (hitPoint.Dot(ray.position) > 0.0f)
                            {
                                if (hit_t < closestIntersection)
                                {
                                    closestIntersection = hit_t;
                                    selectedAxis = static_cast<GizmoAxis>(i + 1);
                                }
                            }
                        }
                    };

                    if (hitInner)
                    {
                        // Si golpea la esfera central, el rayo se divide en dos segmentos por evaluar
                        CheckOverlap(tOut0, tIn0);
                        CheckOverlap(tIn1, tOut1);
                    }
                    else
                    {
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
                if (trackballSphere.Intersects(ray.position, ray.direction, sphereDist) && sphereDist < closestIntersection)
                {
                    closestIntersection = sphereDist;
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
        if (m_transformSpace == TransformSpace::Local || m_currentGizmoType == GizmoType::Scale) 
        {
            // Solo necesitamos la rotación, nos aseguramos de no traer traslaciones del objeto
            baseRotationMatrix = m_selectionState.m_gizmoWorld;
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
