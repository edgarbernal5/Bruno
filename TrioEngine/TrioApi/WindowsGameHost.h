#pragma once

#include "GameHost.h"

namespace TrioEngine
{
	class WindowsGameWindow;

	class WindowsGameHost : public GameHost
	{
	public:
		WindowsGameHost(Game* game);
		~WindowsGameHost();

		void Exit();
		inline Game* GetGame();
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
