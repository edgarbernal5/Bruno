#pragma once

#include "TrioAPI.h"

#include "GameHost.h"
#include "WindowsGameWindow.h"

namespace Cuado
{
	class TRIOAPI_DLL Game;

#ifdef _WIN32 || defined(WIN32)
	class TRIOAPI_DLL WindowsGameHost : public GameHost
	{
	public:
		WindowsGameHost(Game* game);
		~WindowsGameHost();

		void Exit();
		inline Game* GetGame() { return m_pGame; }
		GameWindow* GetWindow();

		void Run();
		void RunOneFrame();
		void StartGameLoop();

		friend class WindowsGameWindow;
	private:
		bool m_bExitRequested;
		Game* m_pGame;

		WindowsGameWindow* m_pWindow;

	};
#endif
}