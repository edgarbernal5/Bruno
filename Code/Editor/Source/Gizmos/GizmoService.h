#pragma once

#include "Bruno/Math/Math.h"
#include "Bruno/Renderer/Camera.h"

namespace Bruno
{
	class GizmoService
	{
	public:
		GizmoService(Camera& camera);

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

		bool BeginDrag(const Math::Vector2& mousePosition);
		void Drag(const Math::Vector2& mousePosition);
		void OnMouseMove(const Math::Vector2& mousePosition);
		void Update();
		void EndDrag();

	private:
		struct SelectionState
		{
			Math::Vector3 m_gizmoPosition;

			float m_screenScaleFactor, m_invScreenScaleFactor;
			Math::Matrix m_screenScaleMatrix;

			Math::Matrix m_gizmoObjectOrientedWorld;
			Math::Matrix m_gizmoAxisAlignedWorld;

			Math::Matrix m_rotationMatrix;

			Math::Vector3 m_intersectionPosition, m_prevIntersectionPosition;
			Math::Vector2 m_prevMousePosition;

			Math::Plane m_currentGizmoPlane;
			Math::Matrix m_objectRotation;
		};

		void InitializeGizmos();

		const float LINE_LENGTH = 3.0f;
		const float CONE_HEIGHT = 0.5f;
		const float CONE_RADIUS = 0.25f;
		const float LINE_OFFSET = 1.0f;
		const float GIZMO_LENGTH = LINE_LENGTH + CONE_HEIGHT;

		const float GIZMO_SCREEN_SCALE = 0.075f;
		const float GIZMO_SIZE_CLIP_SPACE = 2.3f;
		const float CAMERA_GIZMO_SCREEN_SIZE_IN_PIXELS = 60.0f;

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
		
		const Math::Vector3 m_unaryDirections[3]{ Math::Vector3::UnitX, Math::Vector3::UnitY, Math::Vector3::UnitZ };

		Math::Ray ConvertMousePositionToRay(const Math::Vector2& mousePosition);
		GizmoAxis GetAxis(const Math::Vector2& mousePosition);
		bool GetAxisIntersectionPoint(const Math::Vector2& mousePosition, Math::Vector3& intersectionPoint);

		void SetGizmoHandlePlaneFor(GizmoAxis selectedAxis, const Math::Vector2& mousePosition);
		void SetGizmoHandlePlaneFor(GizmoAxis selectedAxis, const Math::Ray& ray);


		Camera& m_camera;
		bool m_isActive = true;
		GizmoType m_currentGizmoType = GizmoType::None;
		PivotType m_pivotType = PivotType::SelectionCenter;
		GizmoAxis m_currentGizmoAxis = GizmoAxis::None;

		Math::Vector3 m_currentDelta;
		SelectionState m_selectionState;
	};
}

