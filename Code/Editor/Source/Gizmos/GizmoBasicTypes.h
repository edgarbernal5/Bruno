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
		float		LineOffset{ Gizmo::LINE_OFFSET };
		uint32_t	Tessellation{ Gizmo::TESSELLATION };
		uint32_t	StackCount{ 3 };

		uint32_t	RingTessellation{ Gizmo::RING_TESSELLATION };
		float		RingThickness{ Gizmo::CONE_RADIUS * 0.5f };

		float		MultiAxisThickness{ Gizmo::MULTI_AXIS_THICKNESS };
		float		SingleAxisThickness{ Gizmo::SINGLE_AXIS_THICKNESS };
	};
	
	// Constantes de transformación (Se pasan al Root Signature del Shader de Gizmos)
	struct GizmoConstants
	{
		Math::Matrix ViewProjection;
	};
	enum class GizmoType
	{
		None,
		Translation,
		Rotation,
		Scale
	};
		
	enum class GizmoAxis
	{
		None,
		X,
		Y,
		Z,
		XY,
		XZ,
		YZ,

		XYZ
	};
		
	enum class PivotType
	{
		ObjectCenter,
		SelectionCenter,
		WorldOrigin
	};
		
	enum class TransformSpace
	{
		Local,
		World
	};
}