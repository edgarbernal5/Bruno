#pragma once

#include "Bruno/Core/Application.h"

#ifdef BR_PLATFORM_WINDOWS

extern Bruno::Application* Bruno::CreateApplication(int argc, char** argv);

namespace Bruno
{
	int Main(int argc, char** argv)
	{
		InitializeCore();
		{
			Bruno::Application* game = Bruno::CreateApplication(argc, argv);
			BR_ASSERT(game, "Client game is null!");
			game->Initialize();
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