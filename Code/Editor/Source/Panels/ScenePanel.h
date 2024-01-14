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
#include "Gizmos/GizmoService.h"

namespace Bruno
{
	class Model;
	class Surface;
	class EditorGame;
	class GraphicsContext;
	class GizmoService;
	class SelectionService;
	class Scene;
	class SceneRenderer;

	//class ScenePanel : public nana::panel<true>
	class ScenePanel : public nana::nested_form
	{
	public:
		ScenePanel(nana::window window, EditorGame* editorGame, std::shared_ptr<Scene> scene, DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT);
		~ScenePanel();

		void OnUpdate(const GameTimer& timer);
		void OnDraw();

		bool IsEnabled();
		std::mutex& GetMutex() { return m_mutex; }

	private:
		void InitializeCamera();
		void InitializeGizmoService();
		void InitializeGraphicsContext();
		void InitializeSceneRenderer();
		void UpdateCBs(const GameTimer& timer);
		Math::Ray ConvertMousePositionToRay(const Math::Int2& mousePosition);

		//std::unique_ptr<nana::nested_form> m_form;
		nana::nested_form* m_form;
		std::unique_ptr<Surface> m_surface;
		int idxx = 0;
		DXGI_FORMAT m_backBufferFormat;
		DXGI_FORMAT m_depthBufferFormat;
		EditorGame* m_editorGame;
		std::shared_ptr<Scene>			m_scene;
		std::shared_ptr<SceneRenderer>	m_sceneRenderer;

		std::shared_ptr<Model> m_model;
		
		std::unique_ptr<GraphicsContext> m_graphicsContext;
		
		std::mutex m_mutex{};

		std::shared_ptr<SelectionService>	m_selectionService;
		std::unique_ptr<GizmoService>	m_gizmoService;

		Camera m_camera;

		Math::Int2 m_lastMousePosition;
		Math::Int2 m_beginMouseDownPosition;
		bool m_isResizing{ false };
		bool m_isSizingMoving{ false };
		bool m_isExposed{ false };

		bool m_shiftPressed{ false };
		bool m_isGizmoing{ false };

		float m_totalTime = 0.0f;
	};
}
