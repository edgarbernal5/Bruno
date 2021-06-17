#pragma once

#include "GameHost.h"

namespace BrunoEngine
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
		bool m_exitRequested;

		Game* m_game;
		WindowsGameWindow* m_window;
	};
}
