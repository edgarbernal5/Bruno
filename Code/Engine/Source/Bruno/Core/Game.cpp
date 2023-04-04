#include "brpch.h"
#include "Game.h"

#include "Bruno/Platform/Windows/WindowsGameWindow.h"
#include "Bruno/Platform/Windows/NanaGameWindow.h"

#include "Bruno/Platform/DirectX/GraphicsAdapter.h"
#include "Bruno/Platform/DirectX/GraphicsDevice.h"

namespace Bruno
{
	Game* Game::g_instance{ nullptr };

	Game::Game(const GameParameters& parameters) :
		m_parameters(parameters),
		m_gameWindow(nullptr)
	{
		g_instance = this;

#if BR_DEBUG
		Device::Core::EnableDebugLayer();
#endif

		m_device = GraphicsDevice::Create();
		Bruno::Graphics::GetDevice() = m_device.get();

		
	}

	Game::~Game()
	{
		delete m_gameWindow;
	}

	void Game::Run()
	{
		DoOnInitialize();
		m_gameWindow->Run();
	}

	void Game::Tick()
	{
	}

	void Game::OnClientSizeChanged()
	{
	}
}
