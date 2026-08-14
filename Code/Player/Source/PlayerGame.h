#pragma once

#include "Bruno.h"

#include "Bruno/Platform/DirectX/VertexTypes.h"
#include "Bruno/Renderer/RHITypes.h"

namespace Bruno
{
	class FrustumCulling;
	class LinearAllocator;
	class Surface;
	class GraphicsContext;
	class Scene;
	class SceneRenderer;
	class CommandQueue;

	class PrimitiveBatch;

	class PlayerGame : public Game
	{
	public:
		PlayerGame(const ApplicationParameters& parameters);

	protected:
		void OnInitialize() override;
		void OnInitializeWindow(const WindowParameters& windowParameters) override;
		void OnResize() override;
		void OnGameLoop(const GameTimer& timer) override;
		void OnUpdate(const GameTimer& timer);
		void OnDraw();

		void OnMouseDown(MouseButtonState btnState, int x, int y) override;
		void OnMouseMove(MouseButtonState btnState, int x, int y) override;
		void OnMouseUp(MouseButtonState btnState, int x, int y) override;
		void OnMouseWheel(MouseButtonState btnState, int x, int y, int wheelDelta) override;

		void OnKeyPressed(KeyCode key, KeyboardState state) override;
		void OnKeyReleased(KeyCode key, KeyboardState state) override;

	private:
		void InitializeCamera();
		void InitializeAllocators();
		void InitializeMeshAndTexture();
		void InitializeSurface();
		void UpdateCBs(const GameTimer& timer);

		std::unique_ptr<Surface> m_surface;
		std::shared_ptr<Scene> m_scene;
		std::shared_ptr<FrustumCulling> m_frustumCulling;
		std::shared_ptr<SceneRenderer> m_sceneRenderer;
		CommandQueue* m_commandQueue { nullptr };
		Math::Viewport m_viewport;
		Rect m_scissorRect;
		ID3D12DescriptorHeap* m_srvHeap;
		std::unique_ptr<GraphicsContext> m_graphicsContext;

		std::array<std::unique_ptr<LinearAllocator>, 2> m_dynamicAllocators;
		Math::Int2	m_lastMousePosition;
		Camera		m_camera;
		bool m_shiftPressed=false;
		bool m_isGizmoing = false;
	};
}
