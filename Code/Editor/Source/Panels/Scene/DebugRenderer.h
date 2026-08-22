#pragma once
#include "Bruno/Math/Math.h"

namespace Bruno
{
    class Camera;
    class GraphicsDevice;
    class GraphicsPipelineState;
    class RootSignature;
    class Scene;
    class PrimitiveBatch;
    class GraphicsContext;

    class DebugRenderer
    {
    public:
        DebugRenderer(GraphicsDevice* device, std::shared_ptr<Scene> scene);
        
        void RenderBoundingBoxes(GraphicsContext* context, const Camera& camera, uint32_t frameIndex);
    
    private:
        std::unique_ptr<PrimitiveBatch> m_primitiveBatch;
		std::shared_ptr<Scene> m_scene;
        std::shared_ptr<RootSignature> m_rootSignature;
        std::unique_ptr<GraphicsPipelineState> m_psoDepthOff;
        GraphicsDevice* m_device;
    };
}
