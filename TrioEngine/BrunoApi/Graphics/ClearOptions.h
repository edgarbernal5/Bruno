#pragma once

#include "TrioApiRequisites.h"

namespace TrioEngine
{
	enum class ClearOptions : int
	{
		Target = 1,
		DepthBuffer = 2,
		Stencil = 4
	};
	DEFINE_ENUM_FLAG_OPERATORS(ClearOptions);
}
