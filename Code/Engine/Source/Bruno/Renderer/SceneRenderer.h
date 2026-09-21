#pragma once

#include <Bruno/Platform/DirectX/RootSignature.h>
#include <Bruno/Platform/DirectX/GraphicsPipelineState.h>
#include <Bruno/Math/Math.h>
#include <entt/entt.hpp>

#include "Bruno/Platform/DirectX/ConstantBuffer.h"
#include "Bruno/Scene/Constants.h"

namespace entt
{
	enum class entity : std::uint32_t;
}

namespace Bruno
{
	class ShadowMapArray;
	class MaterialManager;
	class Material;
	class DescriptorAllocator;
	class GBuffer;
	class CullingSystem;
	class GraphicsContext;
	class Scene;
	class Shader;
	class AbstractAssetManager;
	class Camera;
	
	class SceneRenderer
	{
	public:
		SceneRenderer(std::shared_ptr<Scene> scene, std::shared_ptr<CullingSystem> frustumCulling, AbstractAssetManager* assetManager);
		~SceneRenderer();
		
		// Se llama cuando cargas una escena o agregas un objeto
		void InitEntitiesForRender();
		
		void RenderDeferred(GraphicsContext* context, Camera& camera, uint32_t frameIndex);
		void RenderShadows(GraphicsContext* context, Camera& camera, uint32_t frameIndex);
		
		void Resize(uint32_t width, uint32_t height);
		
		void ExecuteMassiveCulling(const Camera& camera);
	
	private:
		void InitializeShadowArray(GraphicsDevice* device);
		void InitializeShadowPipeline(GraphicsDevice* device);
		
		void RegisterMaterialToGPU(std::shared_ptr<Material> matAsset);
		void DrawBatch(GraphicsContext* graphicsContext, const std::vector<entt::entity>& visibleEntities);
		
		// Función matemática puente
		DirectX::BoundingOrientedBox CreateOBBFromOrthographicMatrix(const Math::Matrix& viewProj);
		void PrepareCullingChunks(uint32_t numChunks, uint32_t chunkSize);
		void ConsolidateFinalLists(uint32_t numChunks);
		
		std::shared_ptr<Scene> m_scene;
		std::shared_ptr<CullingSystem> m_frustumCulling;
		AbstractAssetManager* m_assetManager;

		DescriptorAllocator* m_globalSrvHeap;
		std::shared_ptr<RootSignature> m_shadowRootSig;
        std::shared_ptr<GraphicsPipelineState> m_shadowPSO;
		
		std::unique_ptr<MaterialManager> m_materialManager;
		std::unique_ptr<ShadowMapArray> m_shadowMapArray;
	};
}
