#include "stdafx.h"
#include "Game.h"

#include "Graphics\IGraphicsDeviceManager.h"
#include "Graphics\IGraphicsDeviceService.h"
#include "Graphics\GraphicsDevice.h"

#include "Renderer\RenderPaths\RenderPathForward.h"
#include "Renderer\Renderer.h"
#include "Scene.h"

namespace TrioEngine
{
	Game::Game() :
		m_doneFirstUpdate(false),
		m_doneFirstDraw(false),
		m_isRunning(false),

		m_graphicsDevice(nullptr),
		m_host(nullptr),
		m_graphicsDeviceManager(nullptr)
	{
		CommonConstructor();
	}

	Game::~Game()
	{
		if (m_graphicsDeviceManager)
		{
			delete m_graphicsDeviceManager;
			m_graphicsDeviceManager = nullptr;
		}
	}

	void Game::CommonConstructor()
	{
		m_host = GameHost::Create(this);

		m_host->Idle += ([=]()
		{
			Tick();
		});

		m_host->Activated += [=]()
		{
		};
		m_host->Deactivated += [=]()
		{
		};

		m_host->Resume += [=]()
		{
			m_timer.ResetElapsedTime();
		};

		m_host->GetWindow()->ClientSizeChanged += [=]() {
			auto bounds = m_host->GetWindow()->GetClientBounds();
			m_scene->OnWindowSizeChanged(bounds.width, bounds.height);
		};
	}

	void Game::DrawFrame()
	{
		if (m_doneFirstUpdate && m_graphicsDeviceManager && m_graphicsDeviceManager->BeginDraw())
		{
			Draw(m_timer);
			EndDraw();
			m_doneFirstDraw = true;
		}
	}

	void Game::Draw(StepTimer const& timer)
	{
		m_renderPath->Render();
	}

	void Game::EndDraw()
	{
		if (m_graphicsDeviceManager)
		{
			m_graphicsDeviceManager->EndDraw();
		}
	}

	void Game::Initialize()
	{
		Renderer::Initialize(m_graphicsDevice);

		m_scene = new Scene();
		Scene::SetActiveScene(m_scene);

		auto bounds = m_host->GetWindow()->GetClientBounds();
		m_scene->OnWindowSizeChanged(bounds.width, bounds.height);

		m_renderPath = new RenderPathForward();
	}

	void Game::Run()
	{
		RunGame(true);
	}

	void Game::RunGame(bool useBlockingRun)
	{
		m_graphicsDeviceManager = dynamic_cast<IGraphicsDeviceManager*>(m_services.GetService(typeid(IGraphicsDeviceManager).name()));
		if (m_graphicsDeviceManager != nullptr)
		{
			m_graphicsDeviceManager->CreateDevice();
		}

		IGraphicsDeviceService* service = dynamic_cast<IGraphicsDeviceService*>(m_services.GetService(typeid(IGraphicsDeviceService).name()));
		if (service != nullptr)
		{
			m_graphicsDevice = service->GetGraphicsDevice();
		}
		Initialize();
		m_isRunning = true;
		Update(m_timer);
		m_doneFirstUpdate = true;

		if (useBlockingRun)
		{
			if (m_host)
			{
				m_host->Run();
			}
			//EndRun()
		}
		else
		{
			m_host->StartGameLoop();
			return;
		}

		m_isRunning = false;
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

		m_scene->Update();
	}
}
