#pragma once

#include "TrioApiDecl.h"

namespace TrioEngine
{
	enum class VertexElementUsage
	{
		Position,
		Color,
		TextureCoordinate,
		Normal,
		Binormal,
		Tangent,
		BlendIndices,
		BlendWeight,
		Depth,
		Fog,
		PointSize,
		Sample,
		TessellateFactor
	};
}
