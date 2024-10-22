#include "brpch.h"
#include "Game.h"

#include "Bruno/Platform/DirectX/D3DCore.h"
#include "Bruno/Platform/Windows/WindowsGameWindow.h"
#include "Bruno/Platform/Windows/NanaWindow.h"

#include "Bruno/Platform/DirectX/GraphicsAdapter.h"
#include "Bruno/Platform/DirectX/GraphicsDevice.h"

#include "Bruno/Content/ContentTypeReaderManager.h"

#include <filesystem>
#include <Bruno/Platform/DirectX/Surface.h>

namespace Bruno
{
	Game::Game(const ApplicationParameters& parameters) :
		Application(parameters),
		m_contentManager(parameters.WorkingDirectory)
	{
#if BR_DEBUG
		Device::Core::EnableDebugLayer();
#endif
	}

	Game::~Game()
	{
		m_device->WaitForIdle();

		m_timer.Stop();
		m_window.reset();

#if BR_PLATFORM_WINDOWS
		CoUninitialize();
#endif

		ContentTypeReaderManager::Shutdown();
	}

	void Game::OnInitialize()
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

		ContentTypeReaderManager::Initialize();
		SurfaceWindowParameters surfaceParameters;

		m_shaderCache.Initialize();
		m_psoCache.Initialize(surfaceParameters.BackBufferFormat, surfaceParameters.DepthBufferFormat);
	}

	void Game::OnRun()
	{
		m_timer.Reset();
	}

	void Game::OnTick()
	{
		m_timer.Tick();

		if (m_gamePaused)
		{
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
