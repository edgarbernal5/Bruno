#pragma once

#include <Berta/Controls/Panel.h>
#include <Berta/GUI/Layout.h>
#include <Berta/Controls/Form.h>
#include <Berta/Controls/ComboBox.h>
#include <Berta/Controls/Button.h>

#include <Bruno/Core/GameTimer.h>
#include <Bruno/Renderer/RenderItem.h>
#include <mutex>
#include <Bruno/Platform/DirectX/Texture.h>
#include <Bruno/Renderer/Camera.h>

#include "Bruno/Platform/DirectX/DepthBuffer_Gem.h"
#include "Bruno/Platform/DirectX/Device.h"
#include "Gizmos/GizmoService.h"

namespace Bruno
{
	namespace DX
	{
		class GizmoService;
		class Surface;
	}
	class Model;
	class Surface;
	class EditorGame;
	class GraphicsContext;
	class GizmoService;
	class SelectionService;
	class Scene;
	class SceneDocument;
	class SceneRenderer;
	class CameraGizmo;

	struct SceneSurfaceParameters
	{
		DXGI_FORMAT BackBufferFormat{ DXGI_FORMAT_R8G8B8A8_UNORM };
		DXGI_FORMAT DepthBufferFormat{ DXGI_FORMAT_D32_FLOAT };
	};

	class ScenePanel : public Berta::Panel
	{
	public:
		ScenePanel(Berta::Window* window, EditorGame* editorGame, std::shared_ptr<SceneDocument> sceneDocument, const SceneSurfaceParameters& surfaceParameters = SceneSurfaceParameters());
		~ScenePanel() override;

		void OnUpdate(const GameTimer& timer);
		void OnDraw();

		bool IsEnabled();

#ifndef BR_SINGLE_THREAD_RENDERING
		std::mutex& GetMutex() { return m_mutex; }
#endif

		Berta::NestedForm& GetForm() { return *m_form; }
	private:
		void InitializeGizmoService();
		void InitializeSceneRenderer();
		void SetCameraGizmoViewport();
		void UpdateCBs(const GameTimer& timer);

		std::unique_ptr<Berta::NestedForm> m_form;
		Berta::Layout m_layout;
		Berta::ComboBox m_gizmoTypeCombobox;
		Berta::Button m_gizmoTransformSpaceButton;

		std::unique_ptr<Surface> m_surface;
		int idxx{ 0 };
		SceneSurfaceParameters m_surfaceParameters;
		EditorGame* m_editorGame;
		std::shared_ptr<SceneDocument>		m_sceneDocument;
		std::shared_ptr<Scene>				m_scene;
		SceneRenderer*		m_sceneRenderer { nullptr };

		std::shared_ptr<Model> m_model;
		
		std::unique_ptr<GraphicsContext> m_graphicsContext;

#ifndef BR_SINGLE_THREAD_RENDERING
		std::mutex m_mutex{};
#else
		GameTimer m_timer;
#endif

		std::shared_ptr<SelectionService> m_selectionService;
		std::shared_ptr<DX::GizmoService> m_dxGizmoService;
		std::unique_ptr<CameraGizmo> m_cameraGizmo;

		DX::GraphicsDevice* m_dxDevice;
		std::unique_ptr<DX::Surface> m_dxSurface;
		DX::CommandQueue* m_commandQueue { nullptr };
		Math::Viewport m_viewport;
		D3D12_RECT m_scissorRect;

		ID3D12DescriptorHeap* m_srvHeap;
		
		Math::Int2 m_lastMousePosition;
		Math::Int2 m_beginMouseDownPosition;
		bool m_isResizing{ false };
		bool m_isSizingMoving{ false };
		bool m_isVisible{ false };

		bool m_shiftPressed{ false };
		bool m_isGizmoing{ false };
		bool m_dragRectangle{ false };

		float m_totalTime{ 0.0f };
	};
}
