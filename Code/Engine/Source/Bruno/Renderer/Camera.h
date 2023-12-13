#pragma once

#include "Bruno/Math/Math.h"

namespace Bruno
{
	class Camera
	{
	public:
		Camera();

		const Math::Matrix& GetView();
		const Math::Matrix& GetInverseView();
		const Math::Matrix& GetProjection();
		const Math::Matrix& GetViewProjection();

		bool IsOrthographic() { return m_isOrthographic; }
		float GetNearPlane() { return m_nearPlane; }
		float GetFieldOfView() { return m_fovY; }
		Math::Vector3 GetPosition() { return m_position; }
		Math::Vector3 GetTarget() { return m_target; }
		Math::Viewport GetViewport() { return m_viewport; }
		void SetViewport(const Math::Viewport& viewport);

		void LookAt(const Math::Vector3& position, const Math::Vector3& target, const Math::Vector3& up);
		void SetLens(float nearPlane, float farPlane);
		void SetLens(float fovY, const Math::Viewport& viewport);
		void SetLens(float fovY, const Math::Viewport& viewport, float nearPlane, float farPlane);

		void SetView(const Math::Matrix& viewMatrix);

		void SetIsOrthographic(bool isOrthographic, float size) {
			m_isOrthographic = isOrthographic;
			m_size = size;
			m_states.ProjectionDirty = m_states.ViewProjectionDirty = true;
			
		}
		//TODO: these methods should be in camera controller (!?).
		void HandTool(const Math::Int2& mousePosition, const Math::Int2& previousPosition);
		void Rotate(const Math::Int2& mousePosition, const Math::Int2& previousPosition);
		void PitchYaw(const Math::Int2& mousePosition, const Math::Int2& previousPosition);
		void Zoom(float delta);
		void Strafe(float delta);
		void Walk(float delta);

	private:
		Math::Vector3 m_position;
		Math::Vector3 m_target;
		Math::Vector3 m_up;

		float m_nearPlane;
		float m_farPlane;
		float m_fovY;
		float m_size;
		Math::Viewport m_viewport;

		Math::Matrix m_view;
		Math::Matrix m_inverseView;
		Math::Matrix m_projection;
		Math::Matrix m_viewProjection;
		bool m_isOrthographic{ false };

		struct States
		{
			bool ViewDirty : 1;
			bool ProjectionDirty : 1;
			bool ViewProjectionDirty : 1;
		};
		States m_states{};
	};
}