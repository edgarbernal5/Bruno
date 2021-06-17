#pragma once

#include "TrioApiRequisites.h"

namespace BrunoEngine
{
	enum class SetDataOptions : int
	{
		None,
		Discard = 1,
		NoOverwrite = 2
	};
	DEFINE_ENUM_FLAG_OPERATORS(SetDataOptions);
}
