#pragma once

#include "TrioApiRequisites.h"

namespace TrioEngine
{
	enum class StencilOperation
	{
		Keep = 1,
		Zero = 2,
		Replace = 3,
		Increment = 7,
		Decrement = 8,
		IncrementSaturation = 4,
		DecrementSaturation = 5,
		Invert = 6
	};
}
