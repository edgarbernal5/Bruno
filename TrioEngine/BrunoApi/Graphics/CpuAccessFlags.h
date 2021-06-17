#pragma once

#include "TrioApiRequisites.h"

namespace TrioEngine
{
	enum class CpuAccessFlags
	{
#if BRUNO_DIRECTX
		None = 0,
		Write = D3D11_CPU_ACCESS_WRITE,
		Read = D3D11_CPU_ACCESS_READ,
#else
		None,
		Write,
		Read
#endif
	};
	DEFINE_ENUM_FLAG_OPERATORS(CpuAccessFlags);
}
