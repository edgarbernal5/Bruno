#include "stdafx.h"
#include "GameHost.h"

#include "Game.h"
#include "WindowsGameHost.h"

namespace BrunoEngine
{
	GameHost* GameHost::Create(Game* game)
	{
#ifdef BRUNO_WINDOWS
		return new WindowsGameHost(game);
#elif BRUNO_LINUX
		return nullptr;
#else
		return nullptr;
#endif
	}
}