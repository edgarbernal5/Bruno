#pragma once

#include "Event.h"
#include "GameWindow.h"

namespace TrioEngine
{
	class Game;

	class GameHost
	{
	public:
		static GameHost* Create(Game* game);

		virtual void Exit() = 0;
		virtual void Run() = 0;
		virtual void RunOneFrame() = 0;

		virtual GameWindow* GetWindow() = 0;
		virtual void StartGameLoop() = 0;

		Event<> Activated;
		Event<> Deactivated;
		Event<> Idle;
		Event<> Resume;
		Event<> Suspend;

	protected:

	};

}
