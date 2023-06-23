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
		m_parameters(parameters)
	{
		g_instance = this;

#if BR_DEBUG
		Device::Core::EnableDebugLayer();
#endif

#if BR_PLATFORM_WINDOWS
		HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED); //To be used by DirectXTex.
		if (FAILED(hr))
		{
			BR_CORE_ERROR << "CoInitialize failed." << std::endl;
			throw std::exception("CoInitialize failed.");
		}
#endif

		m_device = GraphicsDevice::Create();
		Bruno::Graphics::GetDevice() = m_device.get();
	}

	Game::~Game()
	{
		//TODO: Flush command queue before changing any resources.
		m_device->GetCommandQueue()->Flush();

		m_gameWindow.reset();

#if BR_PLATFORM_WINDOWS
		CoUninitialize();
#endif
	}

	void Game::Run()
	{
		m_timer.Reset();
		DoOnInitialize();

		m_gameWindow->Run();
	}

	void Game::OnTick()
	{
		m_timer.Tick();
		OnUpdate(m_timer);
		OnDraw();
	}

	void Game::OnClientSizeChanged()
	{
	}
}
