#pragma once

#include "TrioAPI.h"

#include "../Event.h"
namespace Cuado
{
	class GameWindow;
	class Game;

	class GameHost
	{
	public:
		virtual void Exit() = 0;
		virtual void Run() = 0;
		virtual void RunOneFrame() = 0;

		virtual GameWindow* GetWindow() = 0;
		virtual void StartGameLoop() = 0;

		Event<> Idle;

		Event<> Activated;
		Event<> Deactivated;

		Event<> Resume;
		Event<> Suspend;

		static GameHost* Create(Game* game);
	protected:

	};

}