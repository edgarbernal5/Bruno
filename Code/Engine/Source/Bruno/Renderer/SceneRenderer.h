#pragma once

#include <Bruno/Platform/DirectX/RootSignature_Gem.h>
#include <Bruno/Platform/DirectX/GraphicsPipelineState.h>
#include <Bruno/Math/Math.h>

namespace Bruno
{
	class Scene;
	class GraphicsContext;
	class Shader;
	class Surface;
	class AbstractAssetManager;

	class SceneRenderer
	{
	public:
		SceneRenderer(std::shared_ptr<Scene> scene, Surface* surface, AbstractAssetManager* assetManager);

		void OnRender(GraphicsContext* graphicsContext);
	private:
		std::shared_ptr<Scene> m_scene;
		Surface* m_surface;
		AbstractAssetManager* m_assetManager;

		std::shared_ptr<DX::RootSignature> m_opaqueRootSignature;
		std::unique_ptr<Shader> m_opaqueShader;
		std::unique_ptr<DX::GraphicsPipelineState> m_opaquePSO;

	};
}