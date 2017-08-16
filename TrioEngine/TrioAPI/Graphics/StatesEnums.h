#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>

namespace Cuado
{
	enum class CullMode
	{
		None,
		CullClockwiseFace,
		CullCounterClockwiseFace
	};

	enum class FillMode
	{
		Solid,
		WireFrame
	};

	enum class StencilOperation
	{
		Keep = 1,
		Zero = 2,
		Replace = 3,
		Increment = 7,
		Decrement = 8,
		IncrementSaturation = 4,
		DecrementSaturation = 5,
		Invert = 6
	};

	enum class CompareFunction
	{
		Always = 8,
		Never = 1,
		Less = 2,
		LessEqual = 4,
		Equal = 3,
		GreaterEqual = 7,
		Greater = 5,
		NotEqual = 6
	};

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

	enum class Blend
	{
		One,				// Each component of the color is multiplied by (1, 1, 1, 1).
		Zero,	 			// Each component of the color is multiplied by (0, 0, 0, 0).
		SourceColor,		// Each component of the color is multiplied by the source color. This can be represented as (Rs, Gs, Bs, As), where R, G, B, and A respectively stand for the red, green, blue, and alpha source values.
		InverseSourceColor,	// Each component of the color is multiplied by the inverse of the source color. This can be represented as (1 − Rs, 1 − Gs, 1 − Bs, 1 − As) where R, G, B, and A respectively stand for the red, green, blue, and alpha destination values.
		SourceAlpha,		// Each component of the color is multiplied by the alpha value of the source. This can be represented as (As, As, As, As), where As is the alpha source value. 
		InverseSourceAlpha,	// Each component of the color is multiplied by the inverse of the alpha value of the source. This can be represented as (1 − As, 1 − As, 1 − As, 1 − As), where As is the alpha destination value.
		DestinationColor,	//Each component color is multiplied by the destination color. This can be represented as (Rd, Gd, Bd, Ad), where R, G, B, and A respectively stand for red, green, blue, and alpha destination values.
		InverseDestinationColor,	// Each component of the color is multiplied by the inverse of the destination color. This can be represented as (1 − Rd, 1 − Gd, 1 − Bd, 1 − Ad), where Rd, Gd, Bd, and Ad respectively stand for the red, green, blue, and alpha destination values.
		DestinationAlpha,	// Each component of the color is multiplied by the alpha value of the destination. This can be represented as (Ad, Ad, Ad, Ad), where Ad is the destination alpha value.
		InverseDestinationAlpha,	// Each component of the color is multiplied by the inverse of the alpha value of the source. This can be represented as (1 − As, 1 − As, 1 − As, 1 − As), where As is the alpha destination value.
		BlendFactor,		// Each component of the color is multiplied by a constant set in BlendFactor.
		InverseBlendFactor,	//Each component of the color is multiplied by the inverse of a constant set in BlendFactor.
		SourceAlphaSaturation,	// Each component of the color is multiplied by either the alpha of the source color, or the inverse of the alpha of the source color, whichever is greater. This can be represented as (f, f, f, 1), where f = min(A, 1 − Ad).
	};

	enum class ColorWriteChannels
	{
		None = 0,
		Red = 1,
		Green = 2,
		Blue = 4,
		Alpha = 8,
		All = 15
	};
	DEFINE_ENUM_FLAG_OPERATORS(ColorWriteChannels);

	enum class BlendFunction
	{
		Add = 1,
		Subtract = 2,
		ReverseSubtract = 3,
		Max = 5,
		Min = 4,
	};

	enum class ShaderStage
	{
		Vertex,
		Pixel,
		Geometry,
		Compute
	};

}