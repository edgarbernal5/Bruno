#pragma once

#ifdef BR_PLATFORM_WINDOWS

extern Bruno::Game* Bruno::CreateGame(int argc, char** argv);

namespace Bruno
{
	int Main(int argc, char** argv)
	{
		Bruno::Game* game = Bruno::CreateGame(argc, argv);
		game->Run();
		delete game;
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