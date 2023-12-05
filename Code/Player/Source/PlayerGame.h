#pragma once

#include "Bruno/Core/Game.h"
#include "Bruno/Platform/DirectX/Surface.h"
#include "Bruno/Platform/DirectX/Shader.h"
#include "Bruno/Platform/DirectX/RootSignature.h"
#include "Bruno/Platform/DirectX/PipelineStateObject.h"
#include "Bruno/Platform/DirectX/ConstantBuffer.h"
#include "Bruno/Platform/DirectX/Texture.h"
#include "Bruno/Platform/DirectX/VertexTypes.h"

#include "Bruno/Renderer/Camera.h"

namespace Bruno
{
	struct RenderItem;
	class GraphicsContext;
	class Scene;
	class SceneRenderer;
	class GizmoService;
	template<typename TVertex>
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
		void InitializeGraphicsContext();
		void InitializeMeshAndTexture();
		void InitializeShaderAndPipeline();
		void InitializeSurface();
		void UpdateCBs(const GameTimer& timer);

		std::unique_ptr<Surface>		m_surface;
		std::shared_ptr<Scene>			m_scene;
		std::shared_ptr<SceneRenderer>	m_sceneRenderer;
				
		std::unique_ptr<GizmoService>	m_gizmoService;
		std::unique_ptr<GraphicsContext>	m_graphicsContext;

		Math::Int2	m_lastMousePosition;
		Camera		m_camera;
		bool m_shiftPressed=false;
		bool m_isGizmoing = false;
	};
}