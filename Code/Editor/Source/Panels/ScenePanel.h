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
#include <Bruno/Renderer/RenderItem.h>
#include <Bruno/Platform/DirectX/ConstantBuffer.h>
#include <mutex>
#include <Bruno/Platform/DirectX/Texture.h>
#include <Bruno/Renderer/Camera.h>

namespace Bruno
{
	class Surface;
	class EditorGame;

	//class ScenePanel : public nana::panel<true>
	class ScenePanel : public nana::nested_form
	{
	public:
		ScenePanel(nana::window window, EditorGame* editorGame, DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT);
		~ScenePanel();

		void OnUpdate(const GameTimer& timer);
		void OnDraw();

		bool IsEnabled();
		std::mutex& GetMutex() {
			return m_mutex;
		}
	private:
		//std::unique_ptr<nana::nested_form> m_form;
		nana::nested_form* m_form;
		std::unique_ptr<Surface> m_surface;
		int idxx = 0;
		DXGI_FORMAT m_backBufferFormat;
		DXGI_FORMAT m_depthBufferFormat;
		EditorGame* m_editorGame;
		//Scene* m_scene;

		std::unique_ptr<IndexBuffer> m_indexBuffer;
		std::unique_ptr<VertexBuffer> m_vertexBuffer;
		std::unique_ptr<Shader> m_vertexShader;
		std::unique_ptr<Shader> m_pixelShader;
		std::unique_ptr<RootSignature> m_rootSignature;

		std::unique_ptr<PipelineStateObject> m_pipelineState;
		std::shared_ptr<Texture> m_texture;
		//std::unique_ptr<Texture> m_texture;

		std::mutex m_mutex{};

		std::vector<std::shared_ptr<RenderItem>> m_renderItems;

		struct ObjectBuffer
		{
			Math::Matrix World;
		};
		std::unique_ptr<ConstantBuffer<ObjectBuffer>> m_objectBuffer[Graphics::Core::FRAME_BUFFER_COUNT];

		Camera m_camera;

		Math::Int2 m_lastMousePosition;
		bool m_isResizing = false;
		bool m_isSizingMoving = false;
		bool m_isExposed = false;

		float m_totalTime = 0.0f;

		void UpdateCBs(const GameTimer& timer);
	};
}
