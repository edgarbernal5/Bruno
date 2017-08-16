#include "stdafx.h"
#include "GameHost.h"


#include "WindowsGameHost.h"

namespace Cuado
{
	GameHost* GameHost::Create(Game* game)
	{
#ifdef _WINDOWS
		return new WindowsGameHost(game);
#elif _LINUX
		return nullptr;
#else
		return nullptr;
#endif
	}
}