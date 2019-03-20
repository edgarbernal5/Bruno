#pragma once

#include "TrioApiDecl.h"

namespace Vago
{
	enum class DepthStencilViewDimension
	{
		// Resumen:
		//      SharpDX.Direct3D11.DepthStencilViewDimension.Unknown is not a valid value
		//     for SharpDX.Direct3D11.DepthStencilViewDescription and is not used.
		Unknown = 0,
		//
		// Resumen:
		//      The resource will be accessed as a 1D texture.
		Texture1D = 1,
		//
		// Resumen:
		//      The resource will be accessed as an array of 1D textures.
		Texture1DArray = 2,
		//
		// Resumen:
		//      The resource will be accessed as a 2D texture.
		Texture2D = 3,
		//
		// Resumen:
		//      The resource will be accessed as an array of 2D textures.
		Texture2DArray = 4,
		//
		// Resumen:
		//      The resource will be accessed as a 2D texture with multisampling.
		Texture2DMultisampled = 5,
		//
		// Resumen:
		//      The resource will be accessed as an array of 2D textures with multisampling.
		Texture2DMultisampledArray = 6,
	};
}
