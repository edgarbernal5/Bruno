#pragma once

#include "Math/MathVector.h"

namespace BrunoEngine
{
	struct BoundingBoxComponent
	{
		Vector3 m_center;
		Vector3 m_extents;
	};
}