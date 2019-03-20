#include "stdafx.h"
#include "Game.h"

#include "Graphics\IGraphicsDeviceManager.h"
#include "Graphics\IGraphicsDeviceService.h"
#include "Graphics\GraphicsDevice.h"

namespace Vago
{
	Game::Game() :
		m_bDoneFirstUpdate(false),
		m_bDoneFirstDraw(false),
		m_bInRun(false),

		m_pGraphicsDevice(nullptr),
		m_pHost(nullptr),
		m_pGraphicsDeviceManager(nullptr)
	{
		CommonConstructor();
	}

	Game::~Game()
	{
		if (m_pGraphicsDeviceManager)
		{
			delete m_pGraphicsDeviceManager;
			m_pGraphicsDeviceManager = nullptr;
		}
	}

	void Game::CommonConstructor()
	{
		m_pHost = GameHost::Create(this);

		m_pHost->Idle += ([=]()
		{
			Tick();
		});

		m_pHost->Activated += [=]()
		{

		};
		m_pHost->Deactivated += [=]()
		{

		};

		m_pHost->Resume += [=]()
		{

			m_timer.ResetElapsedTime();
		};
	}

	void Game::DrawFrame()
	{
		if (m_bDoneFirstUpdate && m_pGraphicsDeviceManager && m_pGraphicsDeviceManager->BeginDraw())
		{
			Draw(m_timer);
			EndDraw();
			m_bDoneFirstDraw = true;
		}
	}

	void Game::EndDraw()
	{
		if (m_pGraphicsDeviceManager)
		{
			m_pGraphicsDeviceManager->EndDraw();
		}
	}

	void Game::Initialize()
	{
	}


	void Game::Run()
	{
		RunGame(true);
	}

	void Game::RunGame(bool useBlockingRun)
	{
		m_pGraphicsDeviceManager = dynamic_cast<IGraphicsDeviceManager*>(m_services.GetService(typeid(IGraphicsDeviceManager).name()));
		if (m_pGraphicsDeviceManager != nullptr)
		{
			m_pGraphicsDeviceManager->CreateDevice();
		}

		IGraphicsDeviceService* service = dynamic_cast<IGraphicsDeviceService*>(m_services.GetService(typeid(IGraphicsDeviceService).name()));
		if (service != nullptr)
		{
			m_pGraphicsDevice = service->GetGraphicsDevice();
		}
		Initialize();
		m_bInRun = true;
		Update(m_timer);
		m_bDoneFirstUpdate = true;

		if (useBlockingRun)
		{
			if (m_pHost != nullptr)
			{
				m_pHost->Run();
			}
			//EndRun()
		}
		else
		{
			m_pHost->StartGameLoop();
			return;
		}

		m_bInRun = false;
	}

	void Game::Tick()
	{
		m_timer.Tick([&]()
		{
			Update(m_timer);
		});

		DrawFrame();
	}

	void Game::Update(StepTimer const& timer)
	{
		double dt = timer.GetElapsedSeconds();
		double total = timer.GetTotalSeconds();
	}
}
