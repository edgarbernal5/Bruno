#pragma once

#include "TrioApiRequisites.h"

namespace TrioEngine
{
	enum class MapMode : int
	{
#if TRIO_DIRECTX
		Read = D3D11_MAP_READ,
		Write = D3D11_MAP_WRITE,
		ReadWrite = D3D11_MAP_READ_WRITE,
		WriteDiscard = D3D11_MAP_WRITE_DISCARD,
		WriteNoOverwrite = D3D11_MAP_WRITE_NO_OVERWRITE,
#else
		Read,
		Write,
		ReadWrite,
		WriteDiscard,
		WriteNoOverwrite
#endif
	};
	DEFINE_ENUM_FLAG_OPERATORS(MapMode);
}
