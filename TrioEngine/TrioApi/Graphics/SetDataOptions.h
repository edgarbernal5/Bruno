#pragma once

#include "TrioApiRequisites.h"

namespace TrioEngine
{
	enum class SetDataOptions
	{
		None,
		Discard = 1,
		NoOverwrite = 2
	};
	DEFINE_ENUM_FLAG_OPERATORS(SetDataOptions);
}
