#include "PlayerGame.h"

#include "Bruno/Platform/Windows/WindowsGameWindow.h"
#include "Bruno/Platform/DirectX/Surface.h"

#include <iostream>

#include "Bruno/Platform/DirectX/CommandQueue.h"
#include "Bruno/Platform/DirectX/DynamicAllocation.h"
#include "Bruno/Scene/Scene.h"
#include "Bruno/Renderer/SceneRenderer.h"
#include "Bruno/Renderer/PrimitiveBatch.h"
#include "Bruno/Platform/DirectX/GraphicsContext.h"
#include "Bruno/Platform/DirectX/Shader.h"
#include "Bruno/Scene/Systems/FrustumCulling.h"

namespace Bruno
{
	PlayerGame::PlayerGame(const ApplicationParameters& parameters)
		: Game(parameters)
	{
	}

	void PlayerGame::OnInitialize()
	{
		Game::OnInitialize();

		InitializeSurface();
		InitializeMeshAndTexture();
		InitializeCamera();
		InitializeAllocators();
	}

	void PlayerGame::OnInitializeWindow(const WindowParameters& windowParameters)
	{
		m_window = std::make_unique<WindowsGameWindow>(windowParameters, this);
		m_window->Initialize();
		
		m_viewport.height = windowParameters.Height;
		m_viewport.width = windowParameters.Width;
		m_viewport.x = 0;
		m_viewport.y = 0;
		m_viewport.minDepth = D3D12_MIN_DEPTH;
		m_viewport.maxDepth = D3D12_MAX_DEPTH;
		
		m_scissorRect = { 0, 0, static_cast<LONG>(windowParameters.Width), static_cast<LONG>(windowParameters.Height) };
		
	}

	void PlayerGame::OnResize()
	{
		// Resize screen dependent resources.
		m_surface->Resize(m_window->GetWidth(), m_window->GetHeight());
		
		m_viewport.width = m_window->GetWidth();
		m_viewport.height = m_window->GetHeight();
		m_camera.SetViewport(Math::Viewport(0.0f, 0.0f, static_cast<float>(m_window->GetWidth()), static_cast<float>(m_window->GetHeight())));
	}
	
	void PlayerGame::OnGameLoop(const GameTimer& timer)
	{
		OnUpdate(m_timer);
		OnDraw();
	}

	void PlayerGame::OnUpdate(const GameTimer& timer)
	{
		
	}

	void PlayerGame::OnDraw()
	{
		m_timer.Tick();
			
		// 1. Preguntarle al SwapChain en qué frame (0 o 1) estamos trabajando hoy
		uint32_t frameIndex = m_surface->GetCurrentBackBufferIndex();

		//BT_CORE_TRACE << "Scene / delta time = " << m_timer.GetDeltaTime() << ". frameid= "<< frameIndex <<std::endl;
			
		// 2. Pedirle a nuestra cola el "lápiz" (CommandList). 
		// Magia: Esto automáticamente espera si la GPU sigue ocupada con este frame.
		auto commandList = m_commandQueue->GetCommandList(frameIndex);
		auto allocator = m_commandQueue->GetAllocator(frameIndex);
		auto dynamicAllocator = m_dynamicAllocators[frameIndex].get();
		GraphicsContext context(*m_device, commandList.Get(), allocator.Get(), dynamicAllocator);
			
		// 3. Extraer la textura real y su descriptor
		auto backBuffer = m_surface->GetCurrentBackBuffer();
		auto rtvHandle = m_surface->GetCurrentRenderTargetView();

		// ------------------------------------------------------------------
		// FASE DE TRANSICIÓN: PRESENT -> RENDER_TARGET
		// ------------------------------------------------------------------
		context.TransitionResource(backBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			
		// ------------------------------------------------------------------
		// FASE DE DIBUJO
		// ------------------------------------------------------------------
		// Un azul oscuro/grisáceo muy estilo editor AAA (R, G, B, A)
		const float clearColor[] = { 0.10f, 0.014f, 0.16f, 1.0f }; 
		//const float clearColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
		auto dsvHandle = m_surface->GetDepthBufferView();
			
		// Limpiar la pantalla
		context.ClearRenderTarget(rtvHandle, clearColor);
		context.ClearDepth(dsvHandle, 1.0f, 0);
		context.SetRenderTargets(1, &rtvHandle, &dsvHandle);
			
		// Setear SRV Heaps (Indispensable para que la GPU encuentre la textura)
		context.SetDescriptorHeaps(&m_srvHeap, 1);
			
		// Configurar Viewport y Scissor Test explícitamente en este frame
		context.SetViewport(m_viewport);
		context.SetScissorRect(m_scissorRect);
			
		m_sceneRenderer->OnRender(&context, m_camera, frameIndex);
			
		// ------------------------------------------------------------------
		// FASE DE TRANSICIÓN: RENDER_TARGET -> PRESENT
		// ------------------------------------------------------------------
		context.TransitionResource(backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

		// 4. Cerrar el lápiz y enviarlo a la GPU para que lo ejecute
		m_commandQueue->ExecuteCommandList(commandList, frameIndex);

		// 5. Intercambiar los buffers y mostrar en pantalla (VSync activado por ahora)
		m_surface->Present(true);
	}

	void PlayerGame::OnMouseDown(MouseButtonState btnState, int x, int y)
	{
		m_lastMousePosition = Math::Int2(x, y);

		::SetCapture(m_window->GetHandle());
	}

	void PlayerGame::OnMouseMove(MouseButtonState btnState, int x, int y)
	{
		Math::Int2 currentPosition = Math::Int2(x, y);

		if (btnState.LeftButton)
		{
			m_camera.Rotate(currentPosition, m_lastMousePosition);
		}
		else if (btnState.MiddleButton)
		{
			m_camera.HandTool(currentPosition, m_lastMousePosition);
		}
		else if (btnState.RightButton)
		{
			m_camera.PitchYaw(currentPosition, m_lastMousePosition);
		}
		
		m_lastMousePosition = currentPosition;
	}

	void PlayerGame::OnMouseUp(MouseButtonState btnState, int x, int y)
	{
		::ReleaseCapture();
	}

	void PlayerGame::OnMouseWheel(MouseButtonState btnState, int x, int y, int wheelDelta) 
	{
		float zoom = wheelDelta * 0.0025f;
		m_camera.Zoom(zoom);
	}

	void PlayerGame::OnKeyPressed(KeyCode key, KeyboardState state)
	{
		//BR_CORE_TRACE << "key pressed: " << (uint8_t)key << ". alt " << state.Alt << ". ctrl " << state.Ctrl << ". shift " << state.Shift << std::endl;
		if (key == KeyCode::A)
		{
			m_camera.Strafe(-0.25f);
		}
		else if (key == KeyCode::D)
		{
			m_camera.Strafe(0.25f);
		}
		else if (key == KeyCode::W)
		{
			m_camera.Walk(0.25f);
		}
		else if (key == KeyCode::S)
		{
			m_camera.Walk(-0.25f);
		}
		
		m_shiftPressed = (state.Shift);
	}

	void PlayerGame::OnKeyReleased(KeyCode key, KeyboardState state)
	{
		//BR_CORE_TRACE << "key released: " << (uint8_t)key << ". alt " << state.Alt << ". ctrl " << state.Ctrl << ". shift " << state.Shift << std::endl;

		m_shiftPressed = (state.Shift);
	}

	void PlayerGame::InitializeCamera()
	{
		//m_camera.LookAt(Math::Vector3(0, 0, -25), Math::Vector3(0, 0, 0), Math::Vector3(0, 1, 0));
		//m_camera.SetLens(Math::ConvertToRadians(45.0f), Math::Viewport(0.0f, 0.0f, m_surface->GetViewport().Width, m_surface->GetViewport().Height), 1.0f, 1000.0f);
	}

	void PlayerGame::InitializeAllocators()
	{
		for (int i = 0; i < m_dynamicAllocators.size(); ++i)
		{
			m_dynamicAllocators[i] = std::make_unique<LinearAllocator>(*m_device);
		}
	}

	void PlayerGame::InitializeMeshAndTexture()
	{
		m_scene = std::make_shared<Scene>();

		//ContentManager manager(m_applicationParameters.WorkingDirectory);
		//auto model = manager.Load<Model>(L"Models\\Car\\Car.fbx");

		//m_scene->InstantiateModel(model);

		m_frustumCulling = std::make_shared<FrustumCulling>(m_camera, m_scene);
		m_sceneRenderer = std::make_shared<SceneRenderer>(m_scene, m_frustumCulling, m_assetManager.get());
	}

	void PlayerGame::InitializeSurface()
	{
		SurfaceWindowParameters surfaceParameters;
		surfaceParameters.Width = m_applicationParameters.WindowWidth;
		surfaceParameters.Height = m_applicationParameters.WindowHeight;
		surfaceParameters.WindowHandle = reinterpret_cast<HWND>(m_window->GetHandle());

		m_surface = std::make_unique<Surface>(*m_device, surfaceParameters);
		m_commandQueue = &m_device->GetDirectCommandQueue();
		
		m_srvHeap = m_device->GetSRVDescriptorAllocator().GetHeap();
	}

	void PlayerGame::UpdateCBs(const GameTimer& timer)
	{
		m_scene->OnUpdate(timer, m_camera);
	}
}
