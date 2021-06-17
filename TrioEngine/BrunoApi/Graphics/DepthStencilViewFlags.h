#pragma once

#include "TrioApiRequisites.h"

namespace BrunoEngine
{
	enum class DepthStencilViewFlags
	{
		// None.
		None = 0,

		// Indicates that depth values are read only.
		ReadOnlyDepth = 1,

		// Indicates that stencil values are read only.
		ReadOnlyStencil = 2,
	};
}
