#include "stdafx.h"
#include "WindowsGameHost.h"


namespace Cuado
{
#ifdef _WIN32 || defined(WIN32)

	WindowsGameHost::WindowsGameHost(Game* game) : 
		m_pGame(game),
		m_bExitRequested(false)
	{
		m_pWindow = new WindowsGameWindow(this);

		m_pWindow->Activated += ([=]() {
			Activated();
		});

		m_pWindow->Deactivated += ([=]() {
			Deactivated();
		});

		m_pWindow->Resume += ([=]() {
			Resume();
		});

		m_pWindow->Suspend += ([=]() {
			Suspend();
		});

		m_pWindow->GenerateWindow();
	}

	WindowsGameHost::~WindowsGameHost()
	{
	}

	void WindowsGameHost::Exit()
	{
		m_bExitRequested = true;
	}
	
	void WindowsGameHost::StartGameLoop()
	{

	}

	void WindowsGameHost::Run()
	{
		MSG msg = { 0 };
		
		while (msg.message != WM_QUIT)
		{
			// If there are Window messages then process them.
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			// Otherwise, do animation/game stuff.
			else
			{
				if (m_bExitRequested)
				{
					
				}
				else
				{
					RunOneFrame();
				}
			}
		}

		int ret = (int)msg.wParam;
	}

	void WindowsGameHost::RunOneFrame()
	{
		Idle();
	}

	GameWindow* WindowsGameHost::GetWindow()
	{
		return m_pWindow;
	}
#endif

}