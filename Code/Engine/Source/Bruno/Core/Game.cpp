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
		if (m_gameWindow != nullptr) {
			delete m_gameWindow;
			m_gameWindow = nullptr;
		}

#if BR_PLATFORM_WINDOWS
		CoUninitialize();
#endif
	}

	void Game::Run()
	{
		DoOnInitialize();
		m_gameWindow->Run();
	}

	void Game::OnTick(const GameTimer& timer)
	{
		DoOnUpdate(timer);
		DoOnDraw();
	}

	void Game::OnClientSizeChanged()
	{
	}
}
