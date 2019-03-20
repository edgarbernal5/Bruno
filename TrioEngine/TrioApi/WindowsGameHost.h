#pragma once

#include "GameHost.h"

namespace Vago
{
	class WindowsGameWindow;

	class WindowsGameHost : public GameHost
	{
	public:
		WindowsGameHost(Game* game);
		~WindowsGameHost();

		void Exit();
		inline Game* GetGame();
		HWND GetHandle();
		GameWindow* GetWindow();
		
		void Run();
		void RunOneFrame();
		void StartGameLoop();

	private:
		bool m_bExitRequested;

		Game* m_pGame;
		WindowsGameWindow* m_pWindow;
	};
}
