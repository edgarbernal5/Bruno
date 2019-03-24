#pragma once

#include "TrioApiDecl.h"

namespace Vago
{
	enum class MapMode : int
	{
		Read = 1,
		Write = 2,
		ReadWrite = 3,
		WriteDiscard = 4,
		WriteNoOverwrite = 5,
	};
	DEFINE_ENUM_FLAG_OPERATORS(MapMode);
}