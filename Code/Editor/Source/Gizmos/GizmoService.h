#pragma once

#include "Bruno/Renderer/Camera.h"
#include "Constants.h"
#include "GizmoConfig.h"
#include "Bruno/Platform/DirectX/RenderObjectBinding.h"

//#define BR_GIZMO_LINES_INTERSECTION

namespace Bruno
{
	class GizmoTranslationRenderer;
	class GizmoRotationRenderer;
	class GizmoScaleRenderer;
	class GraphicsContext;
	class GraphicsDevice;
	class Surface;
	class SelectionService;
	class Shader;

	struct GizmoObjectBuffer
	{
		Math::Matrix World;
	};

	struct GizmoGraphicsBinding
	{
		std::shared_ptr<Shader> Shader;
	};

	class GizmoService
	{
	public:
		GizmoService(GraphicsDevice* device, Camera& camera, Surface* surface, SelectionService* objectSelector, GizmoConfig gizmoConfig = GizmoConfig());

		using DragTranslationCallback = std::function<void(const Math::Vector3&)>;
		using DragScaleCallback = std::function<void(const Math::Vector3&, bool isUniform)>;
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
		void Render(GraphicsContext* context);
		void Update();
		void EndDrag();

		TransformSpace GetTransformSpace() { return m_transformSpace; }

		void SetTranslationCallback(DragTranslationCallback callback) { m_dragTranslationCallback = callback; }
		void SetScaleCallback(DragScaleCallback callback) { m_dragScaleCallback = callback; }
		void SetRotationCallback(DragRotationCallback callback) { m_dragRotationCallback = callback; }

		void SetGizmoType(GizmoType type){ m_currentGizmoType = type; }
		void SetTransformSpace(TransformSpace space){ m_transformSpace = space; }
		void SetActive(bool isActive){ m_isActive = isActive; }

		void SetGizmoPosition(const Math::Vector3& position) { m_selectionState.m_gizmoPosition = position; }
	private:
		struct SelectionState
		{
			Math::Vector3 m_gizmoPosition;

			float m_screenScaleFactor;
			Math::Matrix m_screenScaleMatrix;

			Math::Matrix m_gizmoObjectOrientedWorld;
			Math::Matrix m_gizmoAxisAlignedWorld;

			Math::Matrix m_rotationMatrix;
			Math::Quaternion m_cameraViewInverseRotation;
			Math::Quaternion m_cameraViewInverseRotationConjugate;

			Math::Vector3 m_intersectionPosition, m_prevIntersectionPosition;
			Math::Vector2 m_prevMousePosition;

			Math::Plane m_currentGizmoPlane;
			bool m_isDragging;
		};

		void UpdateLocalState();

		Math::Vector3 ApplySnapAndPrecisionMode(Math::Vector3 delta);
		Math::Ray ConvertMousePositionToRay(const Math::Vector2& mousePosition);
		Math::Vector3 GetDeltaMovement(const Math::Vector2& mousePosition);
		Math::Quaternion GetRotationDelta(const Math::Vector2& mousePosition);
#ifdef BR_GIZMO_LINES_INTERSECTION
		GizmoAxis GetAxis(const Math::Vector2& mousePosition, Math::Vector3& intersectionPoint);
#else
		GizmoAxis GetAxis(const Math::Vector2& mousePosition);
#endif
		bool GetAxisIntersectionPoint(const Math::Vector2& mousePosition, Math::Vector3& intersectionPoint);
		float GetCameraDistance();
		Math::Vector2 GetScreenPosition(const Math::Vector3& worldPosition);
		void InitializeGizmos();
		void SetGizmoHandlePlaneFor(GizmoAxis selectedAxis, const Math::Vector2& mousePosition);
		void SetGizmoHandlePlaneForRotation(GizmoAxis selectedAxis, const Math::Vector2& mousePosition);
		void SetGizmoHandlePlaneFor(GizmoAxis selectedAxis, const Math::Ray& ray);
		void RenderCameraGizmo(GraphicsContext* context);
#ifdef BR_GIZMO_LINES_INTERSECTION
		std::tuple<Math::Vector3, Math::Vector3> LineLineClosetPoints(Math::Vector3 point1, Math::Vector3 direction1, Math::Vector3 point2, Math::Vector3 direction2);
#endif
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
		Math::Color m_axisColors[3]{ Math::Color(1.0f,0.0f,0.0f,1.0f),Math::Color(0.0f,1.0f,0.0f,1.0f), Math::Color(0.0f,0.0f,1.0f,1.0f) };
		Math::Color m_axisSelectionColor = Math::Color(0.5f, 0.5f, 0.25f, 1);

		Camera& m_camera;
		Camera m_sceneGizmoCamera;
		bool m_isActive{ false };
		GizmoType m_currentGizmoType = GizmoType::Translation;
		PivotType m_pivotType = PivotType::SelectionCenter;
		GizmoAxis m_currentAxis = GizmoAxis::None;
		TransformSpace m_transformSpace = TransformSpace::Local;
		GizmoConfig m_gizmoConfig;

		Math::Vector3 m_currentDelta;
#ifdef BR_GIZMO_LINES_INTERSECTION
		Math::Vector3 m_currentIntersectionPoint;
#endif
		SelectionState m_selectionState;
		DragTranslationCallback m_dragTranslationCallback;
		DragScaleCallback m_dragScaleCallback;
		DragRotationCallback m_dragRotationCallback;

		GizmoGraphicsBinding m_gizmoGraphicsBinding; 
		RenderObjectBinding<GizmoObjectBuffer> m_renderObjectDesc;

		std::shared_ptr<GizmoTranslationRenderer> m_gizmoTranslationRenderer;
		std::shared_ptr<GizmoRotationRenderer> m_gizmoRotationRenderer;
		std::shared_ptr<GizmoScaleRenderer> m_gizmoScaleRenderer;
		std::shared_ptr<GizmoTranslationRenderer> m_gizmoCameraRenderer;
		Surface* m_surface;
		SelectionService* m_selectionService;

		bool m_isSnapEnabled{ false };
		bool m_precisionModeEnabled{ false };
		SnapConfig m_snapConfig;
		Math::Vector3 m_translationScaleSnapDelta;
	};
}

