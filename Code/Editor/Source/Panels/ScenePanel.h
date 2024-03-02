#pragma once

#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/form.hpp>
#include <nana/gui/timer.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/button.hpp>

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

	struct SceneSurfaceParameters
	{
		DXGI_FORMAT BackBufferFormat{ DXGI_FORMAT_R8G8B8A8_UNORM };
		DXGI_FORMAT DepthBufferFormat{ DXGI_FORMAT_D32_FLOAT };
	};

	class ScenePanel : public nana::panel<true>
	//class ScenePanel : public nana::nested_form
	{
	public:
		ScenePanel(nana::window window, EditorGame* editorGame, Camera* camera, std::shared_ptr<Scene> scene, std::shared_ptr<SelectionService> selectionService, std::shared_ptr<GizmoService> gizmoService, const SceneSurfaceParameters& surfaceParameters = SceneSurfaceParameters());
		~ScenePanel();

		void OnUpdate(const GameTimer& timer);
		void OnDraw();

		bool IsEnabled();

#ifndef BR_SINGLE_THREAD_RENDERING
		std::mutex& GetMutex() { return m_mutex; }
#endif

		nana::nested_form& GetForm() { return *m_form; }
	private:
		void InitializeGizmoService();
		void InitializeGraphicsContext();
		void InitializeSceneRenderer();
		void UpdateCBs(const GameTimer& timer);

		std::unique_ptr<nana::nested_form> m_form;
		nana::place m_place;
		nana::combox m_gizmoTypeCombobox;
		nana::button m_gizmoTransformSpaceButton;

		//nana::nested_form* m_form;
		std::unique_ptr<Surface> m_surface;
		int idxx = 0;
		SceneSurfaceParameters m_surfaceParameters;
		EditorGame* m_editorGame;
		std::shared_ptr<Scene>			m_scene;
		std::shared_ptr<SceneRenderer>	m_sceneRenderer;

		std::shared_ptr<Model> m_model;
		
		std::unique_ptr<GraphicsContext> m_graphicsContext;

#ifndef BR_SINGLE_THREAD_RENDERING
		std::mutex m_mutex{};
#endif

		std::shared_ptr<SelectionService>	m_selectionService;
		std::shared_ptr<GizmoService>	m_gizmoService;

		Camera* m_camera;

		Math::Int2 m_lastMousePosition;
		Math::Int2 m_beginMouseDownPosition;
		bool m_isResizing{ false };
		bool m_isSizingMoving{ false };
		bool m_isExposed{ false };

		bool m_shiftPressed{ false };
		bool m_isGizmoing{ false };
		bool m_dragRectangle{ false };

		float m_totalTime = 0.0f;
	};
}
