#include "stdafx.h"
#include "WindowsGameHost.h"

#include "WindowsGameWindow.h"

namespace BrunoEngine
{
	WindowsGameHost::WindowsGameHost(Game* game) :
		m_game(game),
		m_exitRequested(false)
	{
		m_window = new WindowsGameWindow(this);

		m_window->Activated += ([=]() {
			Activated();
		});

		m_window->Deactivated += ([=]() {
			Deactivated();
		});

		m_window->Resume += ([=]() {
			Resume();
		});

		m_window->Suspend += ([=]() {
			Suspend();
		});

		//Load windows after hooked up all windows events.
		m_window->LoadWindow();
	}

	WindowsGameHost::~WindowsGameHost()
	{
	}

	void WindowsGameHost::Exit()
	{
		m_exitRequested = true;
	}

	inline Game* WindowsGameHost::GetGame()
	{
		return m_game;
	}

	GameWindow* WindowsGameHost::GetWindow()
	{
		return m_window;
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
				if (m_exitRequested)
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