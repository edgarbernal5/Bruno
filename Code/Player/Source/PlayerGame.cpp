#include "PlayerGame.h"

#include <Bruno/Platform/Windows/WindowsGameWindow.h>
#include <Bruno/Platform/DirectX/GraphicsAdapter.h>
#include <Bruno/Platform/DirectX/GraphicsDevice.h>
#include <Bruno/Platform/DirectX/Surface.h>
#include <Bruno/Platform/DirectX/CommandQueue.h>
#include <Bruno/Platform/DirectX/ResourceBarrier.h>

namespace Bruno
{
	PlayerGame::PlayerGame(const Bruno::GameParameters& parameters)
		: Game(parameters)
	{
	}

	void PlayerGame::DoOnInitialize()
	{
		GameWindowParameters windowParameters;
		windowParameters.Width = m_parameters.WindowWidth;
		windowParameters.Height = m_parameters.WindowHeight;
		windowParameters.Title = m_parameters.Name;

		m_gameWindow = new WindowsGameWindow(windowParameters, this);
		m_gameWindow->Initialize();

		SurfaceWindowParameters surfaceParameters;
		surfaceParameters.Width= m_parameters.WindowWidth;
		surfaceParameters.Height = m_parameters.WindowHeight;
		surfaceParameters.WindowHandle = reinterpret_cast<HWND>(m_gameWindow->GetHandle());

		m_surface = std::make_unique<Surface>(surfaceParameters);
		m_surface->Initialize();

		m_gameWindow->Show();
	}

	void PlayerGame::Tick()
	{
		m_device->GetCommandQueue()->BeginFrame();

		ID3D12Resource* const currentBackBuffer{ m_surface->GetBackBuffer() };
		ResourceBarrier::Transition(m_device->GetCommandQueue()->GetCommandList(),
			currentBackBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

		float clearColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };

		m_device->GetCommandQueue()->GetCommandList()->ClearRenderTargetView(m_surface->GetRtv(), clearColor, 0, nullptr);
		
		ResourceBarrier::Transition(m_device->GetCommandQueue()->GetCommandList(),
			currentBackBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

		m_device->GetCommandQueue()->EndFrame(m_surface.get());
	}

	void PlayerGame::OnClientSizeChanged()
	{
		m_surface->Resize(m_gameWindow->GetWidth(), m_gameWindow->GetHeight());
	}
}