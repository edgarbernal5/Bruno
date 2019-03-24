#pragma once

#include "TrioApiDecl.h"

namespace Vago
{
	enum class SetDataOptions
	{
		None,
		Discard = 1,
		NoOverwrite = 2
	};
	DEFINE_ENUM_FLAG_OPERATORS(SetDataOptions);
}