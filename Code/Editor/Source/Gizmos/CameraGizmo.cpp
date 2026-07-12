#include "brepch.h"
#include "CameraGizmo.h"

#include "Constants.h"
#include "Bruno/Platform/DirectX/GraphicsContext_Gem.h"

namespace Bruno
{
    CameraGizmo::CameraGizmo(DX::GraphicsDevice* device, Camera& camera) 
        : m_camera(camera), 
        m_device(device),
        m_cameraGizmoBatch(device)
    {
        m_sceneGizmoCamera = m_camera;
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
        struct AxisDef { Math::Color color; Math::Matrix localRot; };
        const AxisDef axes[3] = {
            { m_axisColors[0], Math::Matrix::CreateRotationZ(-Math::PI / 2.0f) }, // X
            { m_axisColors[1], Math::Matrix::Identity },                          // Y
            { m_axisColors[2], Math::Matrix::CreateRotationX(Math::PI / 2.0f) }   // Z
        };

        for (const auto& def : axes)
        {
            // Palo
            m_cameraGizmoBatch.DrawCylinder(Math::Matrix::CreateTranslation(0.0f, length * 0.5f, 0.0f) * def.localRot, 
                                            length, radius, slices, def.color);
            // Punta de Flecha
            m_cameraGizmoBatch.DrawCone(Math::Matrix::CreateTranslation(0.0f, length, 0.0f) * def.localRot, 
                                        arrowH, arrowR, slices, def.color);
        
            // B. Ejes Negativos (-X, -Y, -Z) - Un toque AAA (esferitas opuestas)
            Math::Matrix negMat = Math::Matrix::CreateTranslation(0.0f, -length * 0.8f, 0.0f) * def.localRot;
            m_cameraGizmoBatch.DrawSphere(negMat, radius * 1.5f, slices, slices, def.color);
        }

        // C. Centro (Cubo blanco)
        m_cameraGizmoBatch.DrawBox(Math::Matrix::Identity, radius * 3.0f, Math::Color(1.0f, 1.0f, 1.0f, 1.0f));

        m_cameraGizmoBatch.End(frameIndex);
    }
    
    void CameraGizmo::RenderCameraGizmo(DX::GraphicsContext* context, uint32_t frameIndex, Math::Viewport mainViewport)
    {
        // 1. Guardar el viewport original y configurar el de la esquina (Arriba a la Derecha)
        Math::Viewport gizmoViewport(
            mainViewport.width - Gizmo::CAMERA_GIZMO_SCREEN_SIZE_IN_PIXELS - 10.0f, // 10px de margen
            10, 
            Gizmo::CAMERA_GIZMO_SCREEN_SIZE_IN_PIXELS, 
            Gizmo::CAMERA_GIZMO_SCREEN_SIZE_IN_PIXELS
        );
        context->SetViewport(gizmoViewport);
        m_sceneGizmoCamera.SetLens(m_camera.GetFieldOfView(), gizmoViewport);
        
        // ==========================================
        // 2. MAGIA MATEMÁTICA AAA
        // ==========================================
        auto cameraOrientation = Math::Matrix::CreateFromQuaternion(Math::Quaternion::CreateFromRotationMatrix(m_camera.GetView()));
        m_sceneGizmoCamera.SetView(cameraOrientation * Math::Matrix::CreateLookAt(Math::Vector3(0, 0, Gizmo::GIZMO_LENGTH + 1.0f), Math::Vector3::Zero, Math::Vector3::Up));
        
        // 3. Bindear los Constant Buffers con la nueva matriz
        // 4. Dibujar la geometría previamente agrupada
        RenderBatch(context, frameIndex, m_sceneGizmoCamera.GetViewProjection());

        // 5. Restaurar el viewport principal para no arruinar el UI u otros pases
        context->SetViewport(mainViewport);
    }

    void CameraGizmo::RenderBatch(DX::GraphicsContext* context, uint32_t frameIndex, const Math::Matrix& viewProjection)
    {
    }
}
