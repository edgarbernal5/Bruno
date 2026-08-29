#pragma once

#include <Berta/Controls/Panel.h>
#include <Berta/GUI/Layout.h>
#include <Berta/Controls/Form.h>
#include <Berta/Controls/ComboBox.h>
#include <Berta/Controls/Button.h>

#include <Bruno/Core/GameTimer.h>
#include <mutex>
#include <Bruno/Renderer/Camera.h>

#include "Bruno/Platform/DirectX/DepthBuffer.h"
#include "Bruno/Platform/DirectX/GraphicsDevice.h"
#include "Bruno/Renderer/RHITypes.h"
#include "Gizmos/GizmoService.h"

namespace Bruno
{
	class DebugRenderer;
	class LinearAllocator;
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
		// Estructura para subir al Constant Buffer
		struct MarqueeData
		{
			Math::Vector2 RectMin;
			Math::Vector2 RectMax;
			Math::Color FillColor;
			Math::Color BorderColor;
			float BorderThicknessX;
			float BorderThicknessY;
			float padding[2];
		};
		
		struct MarqueeInteraction
		{
			bool m_dragRectangle{ false };
			
			Math::Vector2 m_ndcMin;
			Math::Vector2 m_ndcMax;
		};
		
		void InitializeGizmoService();
		void InitializeSceneRenderer();
		void InitializeMarquee();
		void SetupCameraGizmoViewport();
		void UpdateCBs(const GameTimer& timer);
		void RenderMarquee(GraphicsContext& context, const Math::Vector2& ndcMin, const Math::Vector2& ndcMax);
		
		std::unique_ptr<Berta::NestedForm> m_form;
		Berta::Layout m_layout;
		Berta::ComboBox m_gizmoTypeCombobox;
		Berta::Button m_gizmoTransformSpaceButton;

		SceneSurfaceParameters m_surfaceParameters;
		EditorGame* m_editorGame;
		std::shared_ptr<SceneDocument> m_sceneDocument;
		std::shared_ptr<Scene> m_scene;
		SceneRenderer* m_sceneRenderer { nullptr };
		
		std::array<std::unique_ptr<LinearAllocator>, 2> m_dynamicAllocators;
		
#ifndef BR_SINGLE_THREAD_RENDERING
		std::mutex m_mutex{};
#else
		GameTimer m_timer;
#endif

		std::shared_ptr<SelectionService> m_selectionService;
		std::shared_ptr<GizmoService> m_gizmoService;
		std::unique_ptr<CameraGizmo> m_cameraGizmo;
		std::shared_ptr<RootSignature> m_marqueeRootSig;
		std::unique_ptr<GraphicsPipelineState> m_marqueePSO;
		std::unique_ptr<DebugRenderer> m_debugRenderer;

		GraphicsDevice* m_device;
		std::unique_ptr<Surface> m_surface;
		CommandQueue* m_commandQueue { nullptr };
		Math::Viewport m_viewport;
		Rect m_scissorRect;

		ID3D12DescriptorHeap* m_srvHeap;
		
		Math::Int2 m_lastMousePosition;
		Math::Int2 m_beginMouseDownPosition;
		bool m_isResizing{ false };
		bool m_isSizingMoving{ false };
		bool m_isVisible{ false };

		bool m_shiftPressed{ false };
		bool m_isGizmoing{ false };
		MarqueeInteraction m_marqueeInteraction;
		float m_totalTime{ 0.0f };
	};
}
