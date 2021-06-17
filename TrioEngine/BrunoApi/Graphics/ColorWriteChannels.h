#pragma once

#include "TrioApiRequisites.h"

namespace TrioEngine
{
	enum class ColorWriteChannels
	{
		None = 0,
		Red = 1,
		Green = 2,
		Blue = 4,
		Alpha = 8,
		All = 15
	};
	DEFINE_ENUM_FLAG_OPERATORS(ColorWriteChannels);
}
