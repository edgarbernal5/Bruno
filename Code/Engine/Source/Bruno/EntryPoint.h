#pragma once

#include "Bruno/Core/Game.h"

#ifdef BR_PLATFORM_WINDOWS

extern Bruno::Game* Bruno::CreateGame(int argc, char** argv);

namespace Bruno
{
	int Main(int argc, char** argv)
	{
		InitializeCore();
		{
			Bruno::Game* game = Bruno::CreateGame(argc, argv);
			BR_ASSERT(game, "Client game is null!");
			game->Run();
			delete game;
		}
		ShutdownCore();

		return 0;
	}
}

int main(int argc, char** argv)
{
	return Bruno::Main(argc, argv);
}

#else

#error Bruno only supports Windows!

#endif