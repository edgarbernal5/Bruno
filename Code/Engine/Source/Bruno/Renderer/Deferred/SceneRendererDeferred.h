#pragma once

namespace Bruno
{
    class Scene;
    class Camera;
    class GraphicsContext;
    class GraphicsPipelineState;
    class RootSignature;
    class GBuffer;

    class SceneRendererDeferred
    {
    public:
        SceneRendererDeferred(GraphicsDevice* device, std::shared_ptr<Scene> scene);
        
        void Render(GraphicsContext* context, Camera& camera, uint32_t frameIndex);
		
        void Resize(uint32_t width, uint32_t height);
    private:
        void InitializeGBuffer(GraphicsDevice* device);
        
        void InitializeGBufferRootSignature(GraphicsDevice* device);
        void InitializeDeferredRootSignature(GraphicsDevice* device);
        void InitializeDeferredPSOs(GraphicsDevice* device);
        
        std::shared_ptr<Scene> m_scene;
        std::shared_ptr<GBuffer> m_gBuffer;
        std::shared_ptr<RootSignature> m_gbufferRootSig;
        std::shared_ptr<RootSignature> m_deferredLightingRootSig;
        
        std::shared_ptr<GraphicsPipelineState> m_gbufferPSO;
        std::shared_ptr<GraphicsPipelineState> m_deferredLightingPSO;
    };
}