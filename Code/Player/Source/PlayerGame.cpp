#include "PlayerGame.h"

#include "Bruno/Platform/Windows/WindowsGameWindow.h"
#include "Bruno/Platform/DirectX/GraphicsAdapter.h"
#include "Bruno/Platform/DirectX/GraphicsDevice.h"
#include "Bruno/Platform/DirectX/Surface.h"
#include "Bruno/Platform/DirectX/CommandQueue.h"
#include "Bruno/Platform/DirectX/VertexTypes.h"
#include "Bruno/Platform/DirectX/ShaderProgram.h"
#include "Bruno/Platform/DirectX/GraphicsContext.h"
#include "Bruno/Content/ContentManager.h"
#include "Bruno/Renderer/RenderItem.h"
#include "Bruno/Content/ContentTypeReaderManager.h"
#include <iostream>
#include "Bruno/Renderer/Model.h"
#include "Bruno/Scene/Scene.h"
#include "Bruno/Renderer/SceneRenderer.h"
#include "Bruno/Renderer/PrimitiveBatch.h"

namespace Bruno
{
	static VertexPositionNormalTexture g_Vertices[24] = {
	VertexPositionNormalTexture{ Math::Vector3(-1.0f, -1.0f, -1.0f), Math::Vector3(0.0f, 0.0f, -1.0f), Math::Vector2(0.0f, 1.0f)},
	VertexPositionNormalTexture{ Math::Vector3(-1.0f, 1.0f, -1.0f), Math::Vector3(0.0f, 0.0f, -1.0f), Math::Vector2(0.0f, 0.0f) },
	VertexPositionNormalTexture{ Math::Vector3(1.0f, 1.0f, -1.0f), Math::Vector3(0.0f, 0.0f, -1.0f), Math::Vector2(1.0f, 0.0f) },
	VertexPositionNormalTexture{ Math::Vector3(1.0f, -1.0f, -1.0f), Math::Vector3(0.0f, 0.0f, -1.0f), Math::Vector2(1.0f, 1.0f) },

	VertexPositionNormalTexture{ Math::Vector3(-1.0f, -1.0f, 1.0f), Math::Vector3(0.0f, 0.0f, 1.0f), Math::Vector2(1.0f, 1.0f)},
	VertexPositionNormalTexture{ Math::Vector3(+1.0f, -1.0f, 1.0f), Math::Vector3(0.0f, 0.0f, 1.0f), Math::Vector2(0.0f, 1.0f) },
	VertexPositionNormalTexture{ Math::Vector3(1.0f, 1.0f, 1.0f), Math::Vector3(0.0f, 0.0f, 1.0f), Math::Vector2(0.0f, 0.0f) },
	VertexPositionNormalTexture{ Math::Vector3(-1.0f, 1.0f, 1.0f), Math::Vector3(0.0f, 0.0f, 1.0f), Math::Vector2(1.0f, 0.0f) },

	VertexPositionNormalTexture{ Math::Vector3(-1.0f, 1.0f, -1.0f), Math::Vector3(0.0f, 1.0f, 0.0f), Math::Vector2(0.0f, 1.0f)},
	VertexPositionNormalTexture{ Math::Vector3(-1.0f, 1.0f, 1.0f), Math::Vector3(0.0f, 1.0f, 0.0f), Math::Vector2(0.0f, 0.0f) },
	VertexPositionNormalTexture{ Math::Vector3(1.0f, 1.0f, 1.0f), Math::Vector3(0.0f, 1.0f, 0.0f), Math::Vector2(1.0f, 0.0f) },
	VertexPositionNormalTexture{ Math::Vector3(1.0f, 1.0f, -1.0f), Math::Vector3(0.0f, 1.0f, 0.0f), Math::Vector2(1.0f, 1.0f) },

	VertexPositionNormalTexture{ Math::Vector3(-1.0f, -1.0f, -1.0f), Math::Vector3(0.0f, -1.0f, 0.0f), Math::Vector2(1.0f, 1.0f)},
	VertexPositionNormalTexture{ Math::Vector3(1.0f, -1.0f, -1.0f), Math::Vector3(0.0f, -1.0f, 0.0f), Math::Vector2(0.0f, 1.0f) },
	VertexPositionNormalTexture{ Math::Vector3(1.0f, -1.0f, 1.0f), Math::Vector3(0.0f, -1.0f, 0.0f), Math::Vector2(0.0f, 0.0f) },
	VertexPositionNormalTexture{ Math::Vector3(-1.0f, -1.0f, 1.0f), Math::Vector3(0.0f, -1.0f, 0.0f), Math::Vector2(1.0f, 0.0f) },

	VertexPositionNormalTexture{ Math::Vector3(-1.0f, -1.0f, 1.0f), Math::Vector3(-1.0f, 0.0f, 0.0f), Math::Vector2(0.0f, 1.0f)},
	VertexPositionNormalTexture{ Math::Vector3(-1.0f, 1.0f, 1.0f), Math::Vector3(-1.0f, 0.0f, 0.0f), Math::Vector2(0.0f, 0.0f) },
	VertexPositionNormalTexture{ Math::Vector3(-1.0f, 1.0f, -1.0f), Math::Vector3(-1.0f, 0.0f, 0.0f), Math::Vector2(1.0f, 0.0f) },
	VertexPositionNormalTexture{ Math::Vector3(-1.0f, -1.0f, -1.0f), Math::Vector3(-1.0f, 0.0f, 0.0f), Math::Vector2(1.0f, 1.0f) },

	VertexPositionNormalTexture{ Math::Vector3(1.0f, -1.0f, -1.0f), Math::Vector3(1.0f, 0.0f, 0.0f), Math::Vector2(0.0f, 1.0f)},
	VertexPositionNormalTexture{ Math::Vector3(1.0f, 1.0f, -1.0f), Math::Vector3(1.0f, 0.0f, 0.0f), Math::Vector2(0.0f, 0.0f) },
	VertexPositionNormalTexture{ Math::Vector3(1.0f, 1.0f, 1.0f), Math::Vector3(1.0f, 0.0f, 0.0f), Math::Vector2(1.0f, 0.0f) },
	VertexPositionNormalTexture{ Math::Vector3(1.0f, -1.0f, 1.0f), Math::Vector3(1.0f, 0.0f, 0.0f), Math::Vector2(1.0f, 1.0f) },

	};

	static uint16_t g_Indices[36] = { 0, 1, 2,
		0, 2, 3,
		4, 5, 6, 
		4, 6, 7,
		8, 9, 10, 
		8, 10, 11,
		12, 13, 14, 
		12, 14, 15, 
		16, 17, 18, 
		16, 18, 19, 
		20, 21, 22,
		20, 22, 23 
	};

	PlayerGame::PlayerGame(const ApplicationParameters& parameters)
		: Game(parameters)
	{
	}

	void PlayerGame::OnInitialize()
	{
		Game::OnInitialize();

		InitializeSurface();
		InitializeMeshAndTexture();
		InitializeShaderAndPipeline();
		InitializeCamera();
		InitializeGraphicsContext();
	}

	void PlayerGame::OnInitializeWindow(const WindowParameters& windowParameters)
	{
		m_window = std::make_unique<WindowsGameWindow>(windowParameters, this);
		m_window->Initialize();
	}

	void PlayerGame::OnResize()
	{
		// Resize screen dependent resources.
		m_surface->Resize(m_window->GetWidth(), m_window->GetHeight());
		
		m_camera.SetViewport(Math::Viewport(0.0f, 0.0f, (float)m_window->GetWidth(), (float)m_window->GetHeight()));
		m_camera.UpdateMatrices();
	}
	
	void PlayerGame::OnGameLoop(const GameTimer& timer)
	{
		OnUpdate(m_timer);
		OnDraw();
	}

	void PlayerGame::OnUpdate(const GameTimer& timer)
	{
		//BR_CORE_TRACE << "delta time = " << timer.GetDeltaTime() << ". TotalTime " << TotalTime << std::endl;
		m_device->BeginFrame();

		UpdateCBs(timer);
	}

	void PlayerGame::OnDraw()
	{
		Math::Color clearColor { 1.0f, 1.0f, 0.0f, 1.0f };

		Texture& backBuffer = m_surface->GetBackBuffer();
		DepthBuffer& depthBuffer = m_surface->GetDepthBuffer();

		m_graphicsContext->Reset();
		m_graphicsContext->AddBarrier(backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_graphicsContext->FlushBarriers();

		m_graphicsContext->ClearRenderTarget(backBuffer, clearColor);
		m_graphicsContext->ClearDepthStencilTarget(depthBuffer, 1.0f, 0);

		m_graphicsContext->SetViewport(m_surface->GetViewport());
		m_graphicsContext->SetScissorRect(m_surface->GetScissorRect());
		
		m_sceneRenderer->OnRender(m_graphicsContext.get());

		m_primitiveBatch->Begin(m_graphicsContext.get());
		m_primitiveBatch->DrawLine(VertexPositionColor(Math::Vector3(0, 0, 0), Math::Color(1, 0, 0, 1)),
			VertexPositionColor(Math::Vector3(3, 0, 0), Math::Color(1, 0, 0, 1)));
		m_primitiveBatch->End();

		m_graphicsContext->AddBarrier(backBuffer, D3D12_RESOURCE_STATE_PRESENT);
		m_graphicsContext->FlushBarriers();

		m_device->SubmitContextWork(*m_graphicsContext);

		m_device->EndFrame();
		m_device->Present(m_surface.get());
	}

	void PlayerGame::OnMouseDown(MouseButtonState btnState, int x, int y)
	{
		m_lastMousePosition = Math::Int2(x, y);

		SetCapture(m_window->GetHandle());
	}

	void PlayerGame::OnMouseMove(MouseButtonState btnState, int x, int y)
	{
		Math::Int2 currentPosition = Math::Int2(x, y);
		if (btnState.LeftButton)
		{
			m_camera.Rotate(currentPosition, m_lastMousePosition);
			m_camera.UpdateMatrices();
		}
		else if (btnState.MiddleButton)
		{
			m_camera.HandTool(currentPosition, m_lastMousePosition);
			m_camera.UpdateMatrices();
		}
		else if (btnState.RightButton)
		{
			m_camera.PitchYaw(currentPosition, m_lastMousePosition);
			m_camera.UpdateMatrices();
		}
		m_lastMousePosition = currentPosition;
	}

	void PlayerGame::OnMouseUp(MouseButtonState btnState, int x, int y)
	{
		ReleaseCapture();
	}

	void PlayerGame::OnMouseWheel(MouseButtonState btnState, int x, int y, int wheelDelta) 
	{
		float zoom = wheelDelta * 0.0025f;
		m_camera.Zoom(zoom);
		m_camera.UpdateMatrices();
	}

	void PlayerGame::OnKeyPressed(KeyCode key, KeyboardState state)
	{
		//BR_CORE_TRACE << "key pressed: " << (uint8_t)key << ". alt " << state.Alt << ". ctrl " << state.Ctrl << ". shift " << state.Shift << std::endl;
		if (key == KeyCode::A)
		{
			m_camera.Strafe(-0.25f);
			m_camera.UpdateMatrices();
		}
		else if (key == KeyCode::D)
		{
			m_camera.Strafe(0.25f);
			m_camera.UpdateMatrices();
		}
		else if (key == KeyCode::W)
		{
			m_camera.Walk(0.25f);
			m_camera.UpdateMatrices();
		}
		else if (key == KeyCode::S)
		{
			m_camera.Walk(-0.25f);
			m_camera.UpdateMatrices();
		}
	}

	void PlayerGame::OnKeyReleased(KeyCode key, KeyboardState state)
	{
		//BR_CORE_TRACE << "key released: " << (uint8_t)key << ". alt " << state.Alt << ". ctrl " << state.Ctrl << ". shift " << state.Shift << std::endl;
	}

	void PlayerGame::InitializeCamera()
	{
		m_camera.LookAt(Math::Vector3(0, 0, -25), Math::Vector3(0, 0, 0), Math::Vector3(0, 1, 0));
		m_camera.SetLens(Math::ConvertToRadians(45.0f), Math::Viewport(0.0f, 0.0f, m_surface->GetViewport().Width, m_surface->GetViewport().Height), 0.1f, 100.0f);
	}

	void PlayerGame::InitializeGraphicsContext()
	{
		GraphicsDevice* device = Graphics::GetDevice();
		m_graphicsContext = std::make_unique<GraphicsContext>(*device);
	}

	void PlayerGame::InitializeMeshAndTexture()
	{
		m_scene = std::make_shared<Scene>(m_camera);

		ContentManager manager(m_applicationParameters.WorkingDirectory);
		auto model = manager.Load<Model>(L"Models\\Car\\Car.fbx");		

		m_scene->AddModel(model);

		m_sceneRenderer = std::make_shared<SceneRenderer>(m_scene, m_surface.get());
	}

	void PlayerGame::InitializeShaderAndPipeline()
	{
		m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(Graphics::GetDevice(), 4096 * 3, 4096);
	}

	void PlayerGame::InitializeSurface()
	{
		SurfaceWindowParameters surfaceParameters;
		surfaceParameters.Width = m_applicationParameters.WindowWidth;
		surfaceParameters.Height = m_applicationParameters.WindowHeight;
		surfaceParameters.WindowHandle = reinterpret_cast<HWND>(m_window->GetHandle());

		m_surface = std::make_unique<Surface>(surfaceParameters);
		m_surface->Initialize();
	}

	void PlayerGame::UpdateCBs(const GameTimer& timer)
	{
		m_scene->OnUpdate(timer);
	}
}