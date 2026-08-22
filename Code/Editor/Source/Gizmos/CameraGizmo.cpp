#include "brepch.h"
#include "CameraGizmo.h"

#include "Constants.h"
#include "GizmoBasicTypes.h"
#include "Bruno/Platform/DirectX/GraphicsContext.h"
#include "Bruno/Platform/DirectX/GraphicsPipelineState.h"
#include "Bruno/Platform/DirectX/RootSignature.h"
#include "Bruno/Platform/DirectX/Shader.h"
#include "Bruno/Platform/DirectX/VertexTypes.h"
#include "Bruno/Renderer/RootSignatureLibrary.h"
#include "Bruno/Renderer/ShaderLibrary.h"

namespace Bruno
{
    CameraGizmo::CameraGizmo(GraphicsDevice* device, Camera& camera) :
        m_camera(camera), 
        m_device(device),
        m_cameraGizmoBatch(device)
    {
        m_sceneGizmoCamera = m_camera;
    }

    void CameraGizmo::Initialize()
    {
        auto prototypeSig = std::make_shared<RootSignature>(*m_device);
        
        m_rootSignature = std::make_unique<RootSignature>(*m_device);

        // 16 floats (32-bits c/u) equivalen a una Matriz de 4x4. 
        // Van al registro b0 y solo el Vertex Shader necesita leerlos.
        prototypeSig->AddConstants(sizeof(Math::Matrix) / 4, 0, 0, ShaderVisibility::Vertex);

        m_rootSignature = RootSignatureLibrary::GetOrCreate(prototypeSig);
        
        ShaderCompileDesc vsDesc = { L"Shaders/UnlitColor.hlsl", L"VSMain", L"vs_6_0" };
        auto vertexShader = ShaderLibrary::GetOrCompile(vsDesc);
        
        ShaderCompileDesc psDesc = { L"Shaders/UnlitColor.hlsl", L"PSMain", L"ps_6_0" };
        auto pixelShader = ShaderLibrary::GetOrCompile(psDesc);
        
        GraphicsPipelineStateDesc psoDesc = {};
        // Definir el Input Layout (DEBE COINCIDIR CON ModelVertex Y CON EL HLSL)
        psoDesc.RootSignature = m_rootSignature.get();
        psoDesc.InputLayout = VertexPositionColor::GetLayout();
        
        psoDesc.VertexShaderDesc = vsDesc;
        psoDesc.PixelShaderDesc = psDesc;
        
        psoDesc.RasterizerState.CullMode = CullMode::None;
    
        psoDesc.DepthState.Mode = DepthMode::None;
        
        psoDesc.BlendState.Mode = BlendMode::AlphaBlend;
        
        psoDesc.Topology = PrimitiveTopology::TriangleList;
        
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = TextureFormat::R8G8B8A8_Unorm;
        psoDesc.DSVFormat = TextureFormat::D24_Unorm_S8_Uint;

        m_psoDepthOff = std::make_unique<GraphicsPipelineState>(*m_device);
        m_psoDepthOff->Initialize(psoDesc);
    }

    void CameraGizmo::BuildCameraGizmoGeometry(uint32_t frameIndex)
    {
        m_cameraGizmoBatch.Begin();

        // El Camera Gizmo suele tener un tamaño constante interno (ej. longitud 1.0)
        const float length = 1.0f;
        const float radius = 0.08f;
        const float arrowH = 0.35f;
        const float arrowR = 0.15f;
        const int   slices = 16;

        // A. Ejes Principales (+X, +Y, +Z)
        struct AxisDef { GizmoAxis axis; Math::Color color; Math::Matrix localRot; };
        const AxisDef axes[3] = {
            {  GizmoAxis::X, m_axisColors[0], Math::Matrix::CreateRotationZ(-Math::PI / 2.0f) }, // X
            { GizmoAxis::Y, m_axisColors[1], Math::Matrix::Identity },                          // Y
            { GizmoAxis::Z,m_axisColors[2], Math::Matrix::CreateRotationX(Math::PI / 2.0f) }   // Z
        };

        Math::Color hoveredColor(1.0f, 1.0f, 0.0f, 1.0f);
        for (const auto& def : axes)
        {
            auto color = def.axis == m_cameraGizmoHoveredAxis ? hoveredColor : def.color;
            
            // Palo
            m_cameraGizmoBatch.DrawCylinder(Math::Matrix::CreateTranslation(0.0f, length * 0.5f, 0.0f) * def.localRot, 
                                            length, radius, slices, color);
            // Punta de Flecha
            m_cameraGizmoBatch.DrawCone(Math::Matrix::CreateTranslation(0.0f, length, 0.0f) * def.localRot, 
                                        arrowH, arrowR, slices, color);
        
            // B. Ejes Negativos (-X, -Y, -Z) - Un toque AAA (esferitas opuestas)
            Math::Matrix negMat = Math::Matrix::CreateTranslation(0.0f, -length * 0.8f, 0.0f) * def.localRot;
            m_cameraGizmoBatch.DrawSphere(negMat, radius * 1.5f, slices, slices, color);
        }

        // C. Centro (Cubo blanco)
        m_cameraGizmoBatch.DrawBox(Math::Matrix::Identity, radius * 3.0f, Math::Color(1.0f, 1.0f, 1.0f, 1.0f));

        m_cameraGizmoBatch.End(frameIndex);
    }
    
    void CameraGizmo::RenderCameraGizmo(GraphicsContext* context, uint32_t frameIndex, Math::Viewport mainViewport)
    {
        // 1. Guardar el viewport original y configurar el de la esquina (Arriba a la Derecha)
        
        context->SetViewport(m_gizmoViewport);
        m_sceneGizmoCamera.SetLens(m_camera.GetFieldOfView(), m_gizmoViewport);
        
        // ==========================================
        // 2. MAGIA MATEMÁTICA AAA
        // ==========================================
        auto cameraOrientation = Math::Matrix::CreateFromQuaternion(Math::Quaternion::CreateFromRotationMatrix(m_camera.GetView()));
        m_sceneGizmoCamera.SetView(cameraOrientation * Math::Matrix::CreateLookAt(Math::Vector3(0, 0, Gizmo::GIZMO_LENGTH + 0.0f), Math::Vector3::Zero, Math::Vector3::Up));
        
        // 3. Bindear los Constant Buffers con la nueva matriz
        // 4. Dibujar la geometría previamente agrupada
        RenderBatch(context, frameIndex, m_sceneGizmoCamera.GetViewProjection());

        // 5. Restaurar el viewport principal para no arruinar el UI u otros pases
        context->SetViewport(mainViewport);
    }

    bool CameraGizmo::OnMouseDown(const Math::Vector2& mousePosition)
    {
        m_mousePressed = false;
        if (!IsMouseOver(mousePosition))
        {
            return false;
        }
        
        Math::Ray ray = CalculateCameraGizmoPickingRay(mousePosition);
        GizmoAxis hoveredAxis = GizmoAxis::None;
        float closestDist = FLT_MAX;

        // Aquí reusas tus datos del BuildGeometry
        const float length = 1.0f; 
        const float radius = 0.08f;

        // Evaluamos intersección contra cada eje (puedes usar bounding boxes simples o cilindros)
        struct AxisDef { GizmoAxis axis; Math::Matrix localRot; };
        const AxisDef axes[3] = {
            { GizmoAxis::X, Math::Matrix::CreateRotationZ(-Math::PI / 2.0f) },
            { GizmoAxis::Y, Math::Matrix::Identity },
            { GizmoAxis::Z, Math::Matrix::CreateRotationX(Math::PI / 2.0f) }
        };

        for (const auto& def : axes)
        {
            // Llevamos el rayo al espacio local de este palo para hacer la intersección más fácil
            Math::Matrix inverseTransform = def.localRot.Invert();
            Math::Ray localRay;
            localRay.position = Math::Vector3::Transform(ray.position, inverseTransform);
            localRay.direction = Math::Vector3::TransformNormal(ray.direction, inverseTransform);

            // Prueba de colisión (ejemplo con BoundingBox de un cilindro genérico)
            Math::BoundingBox stickBox(Math::Vector3(0.0f, length * 0.5f, 0.0f), Math::Vector3(radius, length * 0.5f, radius));
        
            float dist;
            if (localRay.Intersects(stickBox, dist) && dist < closestDist)
            {
                closestDist = dist;
                hoveredAxis = def.axis;
            }
        }

        // Actualizamos el color para que brille en amarillo
        m_cameraGizmoHoveredAxis = hoveredAxis; 

        // ==========================================
        // 4. LA ACCIÓN: SNAP DE LA CÁMARA
        // ==========================================
        if (hoveredAxis != GizmoAxis::None)
        {
            SnapMainCameraToAxis(hoveredAxis);
            m_mousePressed = true;
            return true;
        }
        return false;
    }

    bool CameraGizmo::OnMouseMove(const Math::Vector2& mousePosition)
    {
        if (!IsMouseOver(mousePosition))
        {
            m_cameraGizmoHoveredAxis = GizmoAxis::None;
            return m_mousePressed;
        }
        
        Math::Ray ray = CalculateCameraGizmoPickingRay(mousePosition);
        GizmoAxis hoveredAxis = GizmoAxis::None;
        float closestDist = FLT_MAX;

        // Aquí reusas tus datos del BuildGeometry
        const float length = 1.0f; 
        const float radius = 0.08f;

        // Evaluamos intersección contra cada eje (puedes usar bounding boxes simples o cilindros)
        struct AxisDef { GizmoAxis axis; Math::Matrix localRot; };
        const AxisDef axes[3] = {
            { GizmoAxis::X, Math::Matrix::CreateRotationZ(-Math::PI / 2.0f) },
            { GizmoAxis::Y, Math::Matrix::Identity },
            { GizmoAxis::Z, Math::Matrix::CreateRotationX(Math::PI / 2.0f) }
        };

        for (const auto& def : axes)
        {
            // Llevamos el rayo al espacio local de este palo para hacer la intersección más fácil
            Math::Matrix inverseTransform = def.localRot.Invert();
            Math::Ray localRay;
            localRay.position = Math::Vector3::Transform(ray.position, inverseTransform);
            localRay.direction = Math::Vector3::TransformNormal(ray.direction, inverseTransform);

            // Prueba de colisión (ejemplo con BoundingBox de un cilindro genérico)
            Math::BoundingBox stickBox(Math::Vector3(0.0f, length * 0.5f, 0.0f), Math::Vector3(radius, length * 0.5f, radius));
        
            float dist;
            if (localRay.Intersects(stickBox, dist) && dist < closestDist)
            {
                closestDist = dist;
                hoveredAxis = def.axis;
            }
        }

        // Actualizamos el color para que brille en amarillo
        m_cameraGizmoHoveredAxis = hoveredAxis;
        return m_mousePressed || m_cameraGizmoHoveredAxis != GizmoAxis::None;
        
    }

    bool CameraGizmo::OnMouseUp(const Math::Vector2& mousePosition)
    {
        bool wasPressed = m_mousePressed;
        m_mousePressed = false;
        return wasPressed || m_cameraGizmoHoveredAxis != GizmoAxis::None;
    }

    void CameraGizmo::SetCameraGizmoViewport(const Math::Viewport& viewport)
    {
        m_gizmoViewport = viewport;
    }

    bool CameraGizmo::IsMouseOver(const Math::Vector2& mousePosition)
    {
        // Recreamos el viewport del Camera Gizmo

        return (mousePosition.x >= m_gizmoViewport.x && mousePosition.x <= (m_gizmoViewport.x + m_gizmoViewport.width) &&
                mousePosition.y >= m_gizmoViewport.y && mousePosition.y <= (m_gizmoViewport.y + m_gizmoViewport.height));
    }

    void CameraGizmo::RenderBatch(GraphicsContext* context, uint32_t frameIndex, const Math::Matrix& viewProjection)
    {
        if (m_cameraGizmoBatch.GetIndexCount() == 0)
        {
            return;
        }
        
        context->SetPrimitiveTopology(PrimitiveTopology::TriangleList);
        
        // Bind Root Signature y PSO
        context->SetRootSignature(m_rootSignature.get());
        context->SetPipelineState(m_psoDepthOff.get()); // Usar DepthOff si quieres que flote sobre todo
    
        // Bind Constantes (Root Constants o Constant Buffer temporal)
        GizmoConstants constants = { viewProjection };
        context->SetPushConstants(0, sizeof(GizmoConstants) / 4, &constants, 0);

        // Bind Buffers
        context->SetVertexBuffer(0, m_cameraGizmoBatch.GetVertexBuffer(frameIndex));
        context->SetIndexBuffer(m_cameraGizmoBatch.GetIndexBuffer(frameIndex));

        // DIBUJAR TODO EL BATCH EN 1 SOLO DRAW CALL
        context->DrawIndexedInstanced(m_cameraGizmoBatch.GetIndexCount(), 1, 0, 0, 0);
    }

    Math::Ray CameraGizmo::CalculateCameraGizmoPickingRay(const Math::Vector2& mousePosition)
    {
        // Las mismas matrices que usas en el Render() del Camera Gizmo
        auto cameraOrientation = Math::Matrix::CreateFromQuaternion(Math::Quaternion::CreateFromRotationMatrix(m_camera.GetView()));
        m_sceneGizmoCamera.SetView(cameraOrientation * Math::Matrix::CreateLookAt(Math::Vector3(0, 0, Gizmo::GIZMO_LENGTH + 0.0f), Math::Vector3::Zero, Math::Vector3::Up));
        auto gizmoView = m_sceneGizmoCamera.GetView();
        
        // Tu matriz de proyección legacy
        Math::Matrix gizmoProj = m_sceneGizmoCamera.GetProjection(); 

        // Convertimos las coordenadas (X,Y) de la pantalla a un punto cercano y lejano en el espacio 3D
        Math::Vector3 nearPoint(mousePosition.x, mousePosition.y, 0.0f);
        Math::Vector3 farPoint(mousePosition.x, mousePosition.y, 1.0f);

        Math::Vector3 unprojectedNear = m_gizmoViewport.Unproject(nearPoint, gizmoProj, gizmoView, Math::Matrix::Identity);
        Math::Vector3 unprojectedFar  = m_gizmoViewport.Unproject(farPoint, gizmoProj, gizmoView, Math::Matrix::Identity);

        Math::Vector3 rayDir = unprojectedFar - unprojectedNear;
        rayDir.Normalize();

        return Math::Ray(unprojectedNear, rayDir);
    }

    void CameraGizmo::SnapMainCameraToAxis(GizmoAxis axis)
    {
        // Punto al que queremos mirar (puede ser el centro del mundo o el objeto seleccionado)
        Math::Vector3 targetFocus = m_camera.GetTarget();
        
        // Mantenemos la distancia actual al objetivo
        float currentDistance = (m_camera.GetPosition() - targetFocus).Length();

        Math::Vector3 newDir;
        Math::Vector3 upVector = Math::Vector3::Up;

        switch (axis)
        {
        case GizmoAxis::X: newDir = Math::Vector3::Right;   break; // Mirar desde +X
        case GizmoAxis::Y: newDir = Math::Vector3::Up; upVector = Math::Vector3::Forward; break; // Desde arriba, el Up cambia
        case GizmoAxis::Z: newDir = Math::Vector3::Forward; break; // Mirar desde +Z
        }

        Math::Vector3 newPos = targetFocus + (newDir * currentDistance);
        
        m_camera.LookAt(newPos, targetFocus, upVector);
    }
}
