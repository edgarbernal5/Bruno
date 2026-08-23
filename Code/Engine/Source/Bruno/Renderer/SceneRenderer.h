#pragma once

#include <Bruno/Platform/DirectX/RootSignature.h>
#include <Bruno/Platform/DirectX/GraphicsPipelineState.h>
#include <Bruno/Math/Math.h>

namespace Bruno
{
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
		void InitializeOpaqueRootSignature(GraphicsDevice* device);
		void InitializeOpaquePSO(GraphicsDevice* device);
		void RenderScene(GraphicsContext* graphicsContext, Camera& camera, uint32_t frameIndex);
		
	private:
		
		std::shared_ptr<Scene> m_scene;
		std::shared_ptr<FrustumCulling> m_frustumCulling;
		AbstractAssetManager* m_assetManager;

		std::shared_ptr<RootSignature> m_opaqueRootSignature;
		std::unique_ptr<Shader> m_opaqueShader;
		std::shared_ptr<GraphicsPipelineState> m_opaquePSO;
	};
}