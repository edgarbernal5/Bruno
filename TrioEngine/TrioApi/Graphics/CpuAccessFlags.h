#pragma once

#include "TrioApiDecl.h"

namespace Vago
{
	enum class CpuAccessFlags
	{
		None = 0,
		Write = 65536,
		Read = 131072,
	};
	DEFINE_ENUM_FLAG_OPERATORS(CpuAccessFlags);
}
