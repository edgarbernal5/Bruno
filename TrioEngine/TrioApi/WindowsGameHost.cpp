#include "stdafx.h"
#include "WindowsGameHost.h"

#include "WindowsGameWindow.h"

namespace TrioEngine
{
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

		//Load windows after hooked up all windows events.
		m_pWindow->LoadWindow();
	}

	WindowsGameHost::~WindowsGameHost()
	{
	}

	void WindowsGameHost::Exit()
	{
		m_bExitRequested = true;
	}

	inline Game* WindowsGameHost::GetGame()
	{
		return m_pGame;
	}

	GameWindow* WindowsGameHost::GetWindow()
	{
		return m_pWindow;
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

	void WindowsGameHost::StartGameLoop()
	{

	}
}