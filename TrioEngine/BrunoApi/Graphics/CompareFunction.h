#pragma once

#include "TrioApiRequisites.h"

namespace BrunoEngine
{
	enum class CompareFunction
	{
		Always = 8,
		Never = 1,
		Less = 2,
		LessEqual = 4,
		Equal = 3,
		GreaterEqual = 7,
		Greater = 5,
		NotEqual = 6
	};
}
