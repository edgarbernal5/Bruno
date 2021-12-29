#pragma once

#include "TrioApiRequisites.h"

namespace BrunoEngine
{
	enum class BlendFunction
	{
#ifdef BRUNO_DIRECTX
		Add = D3D11_BLEND_OP_ADD,
		Subtract = D3D11_BLEND_OP_SUBTRACT,
		ReverseSubtract = D3D11_BLEND_OP_REV_SUBTRACT,
		Max = D3D11_BLEND_OP_MAX,
		Min = D3D11_BLEND_OP_MIN,
#else
		Add,
		Subtract,
		ReverseSubtract,
		Max,
		Min,
#endif
	};
}
