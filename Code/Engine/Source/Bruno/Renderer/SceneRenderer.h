#pragma once

#include <Bruno/Platform/DirectX/RootSignature.h>
#include <Bruno/Platform/DirectX/GraphicsPipelineState.h>
#include <Bruno/Math/Math.h>

namespace entt
{
	enum class entity : std::uint32_t;
}

namespace Bruno
{
	class DescriptorAllocator;
	class GBuffer;
	class FrustumCulling;
	class GraphicsContext;
	class Scene;
	class Shader;
	class AbstractAssetManager;
	class Camera;

	class SceneRenderer
	{
	public:
		SceneRenderer(std::shared_ptr<Scene> scene, std::shared_ptr<FrustumCulling> frustumCulling, AbstractAssetManager* assetManager);

		// Se llama cuando cargas una escena o agregas un objeto
		void InitEntitiesForRender();
		
		void RenderForward(GraphicsContext* graphicsContext, Camera& camera, uint32_t frameIndex);
		void RenderDeferred(GraphicsContext* context, Camera& camera, uint32_t frameIndex);
		
		void Resize(uint32_t width, uint32_t height);
		
	private:
		void InitializeGBuffer(GraphicsDevice* device);
		
		void InitializeOpaqueRootSignature(GraphicsDevice* device);
		void InitializeOpaquePSO(GraphicsDevice* device);
		
		void InitializeGBufferRootSignature(GraphicsDevice* device);
		void InitializeDeferredRootSignature(GraphicsDevice* device);
		void InitializeDeferredPSOs(GraphicsDevice* device);
		void InitializeShadowPipeline(GraphicsDevice* device);
		
		void DrawBatch(GraphicsContext* graphicsContext, const std::vector<entt::entity>& visibleEntities);
		
		std::shared_ptr<Scene> m_scene;
		std::shared_ptr<FrustumCulling> m_frustumCulling;
		AbstractAssetManager* m_assetManager;

		DescriptorAllocator* m_globalSrvHeap;
		std::shared_ptr<GBuffer> m_gBuffer;
		std::shared_ptr<RootSignature> m_opaqueRootSignature;
		std::shared_ptr<RootSignature> m_shadowRootSig;
		std::shared_ptr<RootSignature> m_gbufferRootSig;
		std::shared_ptr<RootSignature> m_deferredLightingRootSig;
		
		std::unique_ptr<Shader> m_opaqueShader;
		
		std::shared_ptr<GraphicsPipelineState> m_opaquePSO;
		std::shared_ptr<GraphicsPipelineState> m_shadowPSO;
		std::shared_ptr<GraphicsPipelineState> m_gbufferPSO;
		std::shared_ptr<GraphicsPipelineState> m_deferredLightingPSO;
	};
}
