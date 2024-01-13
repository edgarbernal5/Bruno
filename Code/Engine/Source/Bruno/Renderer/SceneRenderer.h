#pragma once

#include <Bruno/Platform/DirectX/PipelineStateObject.h>
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
		PipelineResourceSpace			m_meshPerObjectResourceSpace;

		std::shared_ptr<RootSignature>			m_rootSignature;
		std::unique_ptr<Shader>					m_opaqueShader;
		std::unique_ptr<PipelineStateObject>	m_pipelineState;

	};
}