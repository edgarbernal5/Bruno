#pragma once

#include <Bruno/Platform/DirectX/RootSignature_Gem.h>
#include <Bruno/Platform/DirectX/GraphicsPipelineState.h>
#include <Bruno/Math/Math.h>

namespace Bruno
{
	namespace DX
	{
		class GraphicsContext;
	}
	
	class Scene;
	class Shader;
	class AbstractAssetManager;
	class Camera;

	class SceneRenderer
	{
	public:
		SceneRenderer(std::shared_ptr<Scene> scene, AbstractAssetManager* assetManager);

		// Se llama cuando cargas una escena o agregas un objeto
		void InitEntitiesForRender();
		void InitializeOpaqueRootSignature(DX::GraphicsDevice* device);
		void InitializeOpaquePSO(DX::GraphicsDevice* device);
		void OnRender(DX::GraphicsContext* graphicsContext, Camera& camera, uint32_t frameIndex);
		
	private:
		std::shared_ptr<Scene> m_scene;
		AbstractAssetManager* m_assetManager;

		std::shared_ptr<DX::RootSignature> m_opaqueRootSignature;
		std::unique_ptr<Shader> m_opaqueShader;
		std::shared_ptr<DX::GraphicsPipelineState> m_opaquePSO;
	};
}