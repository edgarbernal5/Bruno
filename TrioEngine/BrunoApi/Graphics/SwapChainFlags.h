#pragma once

#include "TrioApiRequisites.h"

namespace BrunoEngine
{
	enum class SwapChainFlags : uint32_t
	{
		None = 0,
		AllowModeSwitch = 2,
		AllowTearing = 2048,
	};
	DEFINE_ENUM_FLAG_OPERATORS(SwapChainFlags);
}
