#pragma once
#include "Bruno/Renderer/Camera.h"
#include "Bruno/Renderer/PrimitiveBatch_Gem.h"

namespace Bruno
{
    namespace DX
    {
        class GraphicsDevice;
        class GraphicsContext;
    }

    class CameraGizmo
    {
    public:
        CameraGizmo(DX::GraphicsDevice* device, Camera& camera);
    
        void BuildCameraGizmoGeometry(uint32_t frameIndex);
        void RenderCameraGizmo(DX::GraphicsContext* context, uint32_t frameIndex, Math::Viewport mainViewport);
        
    private:
        void RenderBatch(DX::GraphicsContext* context, uint32_t frameIndex, const Math::Matrix& viewProjection);
        
        Camera& m_camera;
        Camera m_sceneGizmoCamera;
        DX::GraphicsDevice* m_device;
        DX::PrimitiveBatch m_cameraGizmoBatch; // Para la brújula de la esquina
        
        Math::Color m_axisColors[3]{ Math::Color(1.0f,0.0f,0.0f,1.0f),Math::Color(0.0f,1.0f,0.0f,1.0f), Math::Color(0.0f,0.0f,1.0f,1.0f) };
        
        
    };
}
