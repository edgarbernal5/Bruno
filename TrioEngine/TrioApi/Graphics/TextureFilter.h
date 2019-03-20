#pragma once

#include "TrioApiDecl.h"

namespace Vago
{
	enum class TextureFilter
	{
		Linear,			// 	Use linear filtering.
		Point,			// 	Use point filtering.
		Anisotropic,	// 	Use anisotropic filtering.
		LinearMipPoint,	// 	Use linear filtering to shrink or expand, and point filtering between mipmap levels (mip).
		PointMipLinear,	// 	Use point filtering to shrink (minify) or expand (magnify), and linear filtering between mipmap levels.
		MinLinearMagPointMipLinear,	// 	Use linear filtering to shrink, point filtering to expand, and linear filtering between mipmap levels.
		MinLinearMagPointMipPoint,	// 	Use linear filtering to shrink, point filtering to expand, and point filtering between mipmap levels.
		MinPointMagLinearMipLinear,	// 	Use point filtering to shrink, linear filtering to expand, and linear filtering between mipmap levels.
		MinPointMagLinearMipPoint,	// 	Use point filtering to shrink, linear filtering to expand, and point filtering between mipmap levels.
	};
}
