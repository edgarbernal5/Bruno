#pragma once

#include "Bruno/Renderer/Camera.h"
#include "Constants.h"

namespace Bruno
{
	class GizmoTranslationRenderer;
	class GizmoRotationRenderer;
	class GraphicsContext;
	class GraphicsDevice;
	class Surface;
	class ObjectSelector;

	class GizmoService
	{
	public:
		GizmoService(GraphicsDevice* device, Camera& camera, Surface* surface, ObjectSelector* objectSelector);

		using DragTranslationScaleCallback = std::function<void(const Math::Vector3&)>;
		using DragRotationCallback = std::function<void(const Math::Quaternion&)>;

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

		struct SnapConfig
		{
			float Translation{ 1.0f };
			float Scale{ 0.5f };
			float PrecisionScale{ 0.1f };
		};

		bool BeginDrag(const Math::Vector2& mousePosition);
		void Drag(const Math::Vector2& mousePosition);
		void OnMouseMove(const Math::Vector2& mousePosition);
		void OnRender(GraphicsContext* context);
		void Update();
		void EndDrag();

		void SetTranslationCallback(DragTranslationScaleCallback callback) {
			m_dragTranslationCallback = callback;
		}
		void SetScaleCallback(DragTranslationScaleCallback callback) {
			m_dragScaleCallback = callback;
		}
		void SetRotationCallback(DragRotationCallback callback) {
			m_dragRotationCallback = callback;
		}

		void SetGizmoType(GizmoType type) {
			m_currentGizmoType = type;
		}
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

		void UpdateLocalState();

		Math::Vector3 ApplySnapAndPrecisionMode(Math::Vector3 delta);
		Math::Ray ConvertMousePositionToRay(const Math::Vector2& mousePosition);
		Math::Vector3 GetDeltaMovement(const Math::Vector2& mousePosition);
		Math::Quaternion GetRotationDelta(const Math::Vector2& mousePosition);
		GizmoAxis GetAxis(const Math::Vector2& mousePosition);
		bool GetAxisIntersectionPoint(const Math::Vector2& mousePosition, Math::Vector3& intersectionPoint);
		float GetCameraDistance();
		Math::Vector2 GetScreenPosition(const Math::Vector3& worldPosition);
		void InitializeGizmos();

		void SetGizmoHandlePlaneFor(GizmoAxis selectedAxis, const Math::Vector2& mousePosition);
		void SetGizmoHandlePlaneFor(GizmoAxis selectedAxis, const Math::Ray& ray);

		const Math::BoundingBox XAxisBox{
			DirectX::XMFLOAT3((Gizmo::GIZMO_LENGTH + Gizmo::LINE_OFFSET) * 0.5f, 0.0f, 0.0f),
			DirectX::XMFLOAT3((Gizmo::GIZMO_LENGTH - Gizmo::LINE_OFFSET) * 0.5f, Gizmo::SINGLE_AXIS_THICKNESS * 0.5f, Gizmo::SINGLE_AXIS_THICKNESS * 0.5f)
		};

		const Math::BoundingBox YAxisBox{
			DirectX::XMFLOAT3(0.0f, (Gizmo::GIZMO_LENGTH + Gizmo::LINE_OFFSET) * 0.5f, 0.0f),
			DirectX::XMFLOAT3(Gizmo::SINGLE_AXIS_THICKNESS * 0.5f, (Gizmo::GIZMO_LENGTH - Gizmo::LINE_OFFSET) * 0.5f, Gizmo::SINGLE_AXIS_THICKNESS * 0.5f)
		};

		const Math::BoundingBox ZAxisBox{
			DirectX::XMFLOAT3(0.0f, 0.0f, (Gizmo::GIZMO_LENGTH + Gizmo::LINE_OFFSET) * 0.5f),
			DirectX::XMFLOAT3(Gizmo::SINGLE_AXIS_THICKNESS * 0.5f, Gizmo::SINGLE_AXIS_THICKNESS * 0.5f, (Gizmo::GIZMO_LENGTH - Gizmo::LINE_OFFSET) * 0.5f)
		};

		const Math::BoundingBox XZAxisBox{
			DirectX::XMFLOAT3(Gizmo::LINE_OFFSET * 0.5f, Gizmo::MULTI_AXIS_THICKNESS * 0.5f, Gizmo::LINE_OFFSET * 0.5f),
			DirectX::XMFLOAT3(Gizmo::LINE_OFFSET * 0.5f, Gizmo::MULTI_AXIS_THICKNESS * 0.5f, Gizmo::LINE_OFFSET * 0.5f)
		};

		const Math::BoundingBox XYAxisBox{
			DirectX::XMFLOAT3(Gizmo::LINE_OFFSET * 0.5f, Gizmo::LINE_OFFSET * 0.5f, Gizmo::MULTI_AXIS_THICKNESS * 0.5f),
			DirectX::XMFLOAT3(Gizmo::LINE_OFFSET * 0.5f, Gizmo::LINE_OFFSET * 0.5f, Gizmo::MULTI_AXIS_THICKNESS * 0.5f)
		};

		const Math::BoundingBox YZAxisBox{
			DirectX::XMFLOAT3(Gizmo::MULTI_AXIS_THICKNESS * 0.5f, Gizmo::LINE_OFFSET * 0.5f, Gizmo::LINE_OFFSET * 0.5f),
			DirectX::XMFLOAT3(Gizmo::MULTI_AXIS_THICKNESS * 0.5f, Gizmo::LINE_OFFSET * 0.5f, Gizmo::LINE_OFFSET * 0.5f)
		};

		const Math::BoundingBox XYZAxisBox{
			DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
			DirectX::XMFLOAT3(Gizmo::LINE_LENGTH * 0.25f, Gizmo::LINE_LENGTH * 0.25f, Gizmo::LINE_LENGTH * 0.25f)
		};
		
		const Math::Vector3 m_unaryDirections[3]{ Math::Vector3::UnitX, Math::Vector3::UnitY, Math::Vector3::UnitZ };
		Math::Color m_activeAxisColors[3];
		Math::Color m_axisColors[3]{ Math::Color(1.0f,0,0,1),Math::Color(0,1.0f,0,1) , Math::Color(0,0,1.0f,1) };
		Math::Color m_axisSelectionColor = Math::Color(0.5f, 0.5f, 0.25f, 1);

		Camera& m_camera;
		bool m_isActive{ true };
		GizmoType m_currentGizmoType = GizmoType::Rotation;
		PivotType m_pivotType = PivotType::SelectionCenter;
		GizmoAxis m_currentGizmoAxis = GizmoAxis::None;
		TransformSpace m_transformSpace = TransformSpace::World;

		Math::Vector3 m_currentDelta;
		SelectionState m_selectionState;
		DragTranslationScaleCallback m_dragTranslationCallback;
		DragTranslationScaleCallback m_dragScaleCallback;
		DragRotationCallback m_dragRotationCallback;

		std::shared_ptr<GizmoTranslationRenderer> m_gizmoTranslationRenderer;
		std::shared_ptr<GizmoRotationRenderer> m_gizmoRotationRenderer;
		Surface* m_surface;
		ObjectSelector* m_objectSelector;

		bool m_isSnapEnabled{ false };
		bool m_precisionModeEnabled{ false };
		SnapConfig m_snapConfig;
		Math::Vector3 m_translationScaleSnapDelta;
	};
}

