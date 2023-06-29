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

	private:
		std::unique_ptr<Surface> m_surface;
		std::unique_ptr<Shader> m_vertexShader;
		std::unique_ptr<Shader> m_pixelShader;
		std::unique_ptr<Texture> m_texture;
		std::unique_ptr<RootSignature> m_rootSignature;

		std::unique_ptr<PipelineStateObject> m_pipelineState;

		std::vector<std::shared_ptr<RenderItem>> m_renderItems;

		struct ObjectBuffer
		{
			Math::Matrix World;
		};
		std::unique_ptr<ConstantBuffer<ObjectBuffer>> m_objectBuffer[Graphics::Core::FRAME_BUFFER_COUNT];

		Math::Int2 m_lastMousePosition;
		Camera m_camera;

		void UpdateCBs(const GameTimer& timer);
	};
}