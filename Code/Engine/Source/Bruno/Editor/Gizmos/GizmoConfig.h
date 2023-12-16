#pragma once
#include <cstdint>
#include "Constants.h"

namespace Bruno
{
	struct GizmoConfig
	{
		float		StickRadius{ Gizmo::STICK_RADIUS };
		float		StickHeight{ Gizmo::LINE_LENGTH };
		float		ArrowheadRadius{ Gizmo::CONE_RADIUS };
		float		ArrowheadHeight{ Gizmo::CONE_HEIGHT };
		uint32_t	Tessellation{ Gizmo::TESSELLATION };
		uint32_t	StackCount{ 3 };

		uint32_t	RingTessellation{ Gizmo::RING_TESSELLATION };
		float		RingThickness{ Gizmo::CONE_RADIUS };

		float		MultiAxisThickness{ Gizmo::MULTI_AXIS_THICKNESS };
		float		SingleAxisThickness{ Gizmo::SINGLE_AXIS_THICKNESS };
	};

}