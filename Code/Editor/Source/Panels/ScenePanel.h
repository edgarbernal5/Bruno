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
#include <Bruno/Core/GameTimer.h>
#include <mutex>

namespace Bruno
{
	class Surface;
	class EditorGame;

	class ScenePanel : public nana::nested_form
	{
	public:
		ScenePanel(nana::window window, EditorGame* editorGame, DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT);
		~ScenePanel();

		void OnUpdate(const GameTimer& timer);
		void OnDraw();

	private:
		std::unique_ptr<Surface> m_surface;
		int idxx = 0;
		DXGI_FORMAT m_backBufferFormat;
		DXGI_FORMAT m_depthBufferFormat;
		//Scene* m_scene;

		std::unique_ptr<IndexBuffer> m_indexBuffer;
		std::unique_ptr<VertexBuffer> m_vertexBuffer;
		std::unique_ptr<Shader> m_vertexShader;
		std::unique_ptr<Shader> m_pixelShader;
		std::unique_ptr<RootSignature> m_rootSignature;

		std::unique_ptr<PipelineStateObject> m_pipelineState;

		std::mutex m_mutex{};
		bool m_isResizing = false;
		bool m_isSizingMoving = false;
		bool m_isExposed = false;

		float m_totalTime = 0.0f;
	};
}
