#pragma once

#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/form.hpp>
#include <nana/gui/timer.hpp>

#include <Bruno/Platform/DirectX/IndexBuffer.h>
#include <Bruno/Platform/DirectX/VertexBuffer.h>
#include <Bruno/Platform/DirectX/Shader.h>
#include <Bruno/Platform/DirectX/RootSignature.h>
#include <Bruno/Platform/DirectX/PipelineStateObject.h>

namespace Bruno
{
	class Surface;

	class ScenePanel : public nana::nested_form
	{
	public:
		ScenePanel(nana::window window);
		~ScenePanel() = default;

	private:
		std::unique_ptr<Surface> m_surface;
		int idxx = 0;
		//Scene* m_scene;

		std::unique_ptr<IndexBuffer> m_indexBuffer;
		std::unique_ptr<VertexBuffer> m_vertexBuffer;
		std::unique_ptr<Shader> m_vertexShader;
		std::unique_ptr<Shader> m_pixelShader;
		std::unique_ptr<RootSignature> m_rootSignature;

		std::unique_ptr<PipelineStateObject> m_pipelineState;

		nana::timer m_timer;
		float m_totalTime = 0.0f;
	};
}
