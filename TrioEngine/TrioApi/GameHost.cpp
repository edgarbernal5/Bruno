#include "stdafx.h"
#include "GameHost.h"

#include "Game.h"
#include "WindowsGameHost.h"

namespace Vago
{
	GameHost* GameHost::Create(Game* game)
	{
#ifdef TRIO_WINDOWS
		return new WindowsGameHost(game);
#elif TRIO_LINUX
		return nullptr;
#else
		return nullptr;
#endif
	}
}