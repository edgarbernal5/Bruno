#pragma once

#include "Bruno/Math/Math.h"

namespace Bruno
{
	class GizmoService
	{
	public:


	private:
		const float LINE_LENGTH = 3.0f;
		const float CONE_HEIGHT = 0.5f;
		const float CONE_RADIUS = 0.25f;
		const float LINE_OFFSET = 1.0f;
		const float GIZMO_LENGTH = LINE_LENGTH + CONE_HEIGHT;

		const float MULTI_AXIS_THICKNESS = 0.05f;
		const float SINGLE_AXIS_THICKNESS = 0.35f;

		const Math::BoundingBox XAxisBox{
			DirectX::XMFLOAT3((GIZMO_LENGTH + LINE_OFFSET) * 0.5f, 0.0f, 0.0f),
			DirectX::XMFLOAT3((GIZMO_LENGTH - LINE_OFFSET) * 0.5f, SINGLE_AXIS_THICKNESS * 0.5f, SINGLE_AXIS_THICKNESS * 0.5f)
		};

		const Math::BoundingBox YAxisBox{
			DirectX::XMFLOAT3(0.0f, (GIZMO_LENGTH + LINE_OFFSET) * 0.5f, 0.0f),
			DirectX::XMFLOAT3(SINGLE_AXIS_THICKNESS * 0.5f, (GIZMO_LENGTH - LINE_OFFSET) * 0.5f, SINGLE_AXIS_THICKNESS * 0.5f)
		};

		const Math::BoundingBox ZAxisBox{
			DirectX::XMFLOAT3(0.0f, 0.0f, (GIZMO_LENGTH + LINE_OFFSET) * 0.5f),
			DirectX::XMFLOAT3(SINGLE_AXIS_THICKNESS * 0.5f, SINGLE_AXIS_THICKNESS * 0.5f, (GIZMO_LENGTH - LINE_OFFSET) * 0.5f)
		};

		const Math::BoundingBox XZAxisBox{
			DirectX::XMFLOAT3(LINE_OFFSET * 0.5f, MULTI_AXIS_THICKNESS * 0.5f, LINE_OFFSET * 0.5f),
			DirectX::XMFLOAT3(LINE_OFFSET * 0.5f, MULTI_AXIS_THICKNESS * 0.5f, LINE_OFFSET * 0.5f)
		};

		const Math::BoundingBox XYBox{
			DirectX::XMFLOAT3(LINE_OFFSET * 0.5f, LINE_OFFSET * 0.5f, MULTI_AXIS_THICKNESS * 0.5f),
			DirectX::XMFLOAT3(LINE_OFFSET * 0.5f, LINE_OFFSET * 0.5f, MULTI_AXIS_THICKNESS * 0.5f)
		};

		const Math::BoundingBox YZBox{
			DirectX::XMFLOAT3(MULTI_AXIS_THICKNESS * 0.5f, LINE_OFFSET * 0.5f, LINE_OFFSET * 0.5f),
			DirectX::XMFLOAT3(MULTI_AXIS_THICKNESS * 0.5f, LINE_OFFSET * 0.5f, LINE_OFFSET * 0.5f)
		};

		const Math::BoundingBox XYZBox{
			DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
			DirectX::XMFLOAT3(LINE_LENGTH * 0.25f, LINE_LENGTH * 0.25f, LINE_LENGTH * 0.25f)
		};
	};
}

