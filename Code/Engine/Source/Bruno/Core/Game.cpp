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
	}

	Game::~Game()
	{
		//TODO: Flush command queue before changing any resources.
		m_device->GetCommandQueue()->Flush();

		m_timer.Stop();
		m_gameWindow.reset();

#if BR_PLATFORM_WINDOWS
		CoUninitialize();
#endif
	}

	void Game::Initialize()
	{
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

		GameWindowParameters windowParameters;
		windowParameters.Width = m_parameters.WindowWidth;
		windowParameters.Height = m_parameters.WindowHeight;
		windowParameters.Title = m_parameters.Name;
		OnInitialize(windowParameters);
	}

	void Game::Run()
	{
		m_timer.Reset();

		m_gameWindow->Run();
	}

	void Game::OnTick()
	{
		m_timer.Tick();

		if (m_gamePaused)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			return;
		}

		//TODO: improve this timer with a high precision timer.
		m_framesThisSecond++;
		if (m_timer.GetTotalTime() - m_elapsedTime >= 1.0f)
		{
			m_framesPerSecond = m_framesThisSecond;
			m_framesThisSecond = 0;
			m_elapsedTime += 1.0f;

			//BR_CORE_TRACE << "FPS: " << m_framesPerSecond << std::endl;
		}

		OnGameLoop(m_timer);
	}

	void Game::OnActivated()
	{
		m_gamePaused = false;
		m_timer.Start();
	}

	void Game::OnDeactivated()
	{
		m_gamePaused = true; 
		m_timer.Stop();
	}

	void Game::OnResizeMoveStarted()
	{
		if (m_gamePaused)
			return;

		m_gamePaused = true;
		m_timer.Stop();
	}

	void Game::OnResizeMoveFinished()
	{
		if (!m_gamePaused)
			return;

		m_gamePaused = false;
		m_timer.Start();
	}
}
