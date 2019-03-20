#pragma once

#include "TrioApiDecl.h"

namespace Vago
{
	enum class DepthWriteMask
	{
		// Resumen:
		//      Turn off writes to the depth-stencil buffer.
		Zero = 0,
		//
		// Resumen:
		//      Turn on writes to the depth-stencil buffer.
		All = 1,
	};
}
