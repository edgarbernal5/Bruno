#pragma once

#include <Bruno.h>
#include <Bruno/Platform/DirectX/Surface.h>
#include <Bruno/Platform/DirectX/Shader.h>
#include <Bruno/Platform/DirectX/RootSignature.h>
#include <Bruno/Platform/DirectX/PipelineStateObject.h>
#include <Bruno/Platform/DirectX/ConstantBuffer.h>
#include <Bruno/Platform/DirectX/Texture.h>

#include <Bruno/Renderer/Camera.h>

namespace Bruno
{
	struct RenderItem;
	class GraphicsContext;

	class PlayerGame : public Game
	{
	public:
		PlayerGame(const GameParameters& parameters);

	protected:
		void OnInitialize(const GameWindowParameters& windowParameters) override;
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
		void UpdateCBs(const GameTimer& timer);

		std::unique_ptr<Surface> m_surface;
		std::shared_ptr<Texture> m_texture;
		std::shared_ptr<RootSignature> m_rootSignature;

		std::unique_ptr<Shader> m_opaqueShader;
		
		std::unique_ptr<PipelineStateObject> m_pipelineState;
		std::unique_ptr<GraphicsContext> m_graphicsContext;

		std::vector<std::shared_ptr<RenderItem>> m_renderItems;
		PipelineResourceSpace mMeshPerObjectResourceSpace;

		struct ObjectBuffer
		{
			Math::Matrix World;
		};
		std::unique_ptr<ConstantBuffer<ObjectBuffer>> m_objectBuffer[Graphics::Core::FRAMES_IN_FLIGHT_COUNT];

		Math::Int2 m_lastMousePosition;
		Camera m_camera;
	};
}