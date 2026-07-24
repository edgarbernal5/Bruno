#pragma once
#include "GizmoBasicTypes.h"
#include "Bruno/Renderer/Camera.h"
#include "Bruno/Renderer/PrimitiveBatch.h"

namespace Bruno
{
    class GraphicsPipelineState;
    class RootSignature;
    class GraphicsDevice;
    class GraphicsContext;

    class CameraGizmo
    {
    public:
        CameraGizmo(GraphicsDevice* device, Camera& camera);
    
        void Initialize();
        void BuildCameraGizmoGeometry(uint32_t frameIndex);
        void RenderCameraGizmo(GraphicsContext* context, uint32_t frameIndex, Math::Viewport mainViewport);
        
        bool OnMouseDown(const Math::Vector2& mousePosition);
        bool OnMouseMove(const Math::Vector2& mousePosition);
        bool OnMouseUp(const Math::Vector2& mousePosition);
        void SetCameraGizmoViewport(const Math::Viewport& viewport);
        
    private:
        bool IsMouseOver(const Math::Vector2& mousePosition);
        void RenderBatch(GraphicsContext* context, uint32_t frameIndex, const Math::Matrix& viewProjection);
        Math::Ray CalculateCameraGizmoPickingRay(const Math::Vector2& mousePosition);
        
        void SnapMainCameraToAxis(GizmoAxis axis);
        
        GraphicsDevice* m_device;
        Camera& m_camera;
        Camera m_sceneGizmoCamera;
        Math::Viewport m_gizmoViewport;
        GizmoAxis m_cameraGizmoHoveredAxis { GizmoAxis::None };
        bool m_mousePressed = false;
        PrimitiveBatch m_cameraGizmoBatch; // Para la brújula de la esquina
        std::unique_ptr<RootSignature> m_rootSignature;
        std::unique_ptr<GraphicsPipelineState> m_psoDepthOff; // Gizmos "X-Ray" dibujados sobre los objetos

        Math::Color m_axisColors[3]{ Math::Color(1.0f,0.0f,0.0f,1.0f),Math::Color(0.0f,1.0f,0.0f,1.0f), Math::Color(0.0f,0.0f,1.0f,1.0f) };
    };
}
