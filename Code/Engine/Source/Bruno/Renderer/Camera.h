#pragma once

#include "Bruno/Math/Math.h"

namespace Bruno
{
	class Camera
	{
	public:
		Camera();

		const Math::Matrix& GetView() const;
		const Math::Matrix& GetViewInverse() const;
		const Math::Matrix& GetProjection() const;
		const Math::Matrix& GetViewProjection() const;

		bool IsOrthographic() const { return m_isOrthographic; }
		float GetNearPlane() const { return m_nearPlane; }
		float GetFieldOfView() const { return m_fovY; }
		Math::Vector3 GetPosition() const { return m_position; }
		Math::Vector3 GetTarget() const { return m_target; }
		Math::Vector3 GetUp() const { return m_up; }
		Math::Viewport GetViewport() const { return m_viewport; }
		
		void SetTarget(const Math::Vector3& target);

		void SetPosition(const Math::Vector3& position);
		
		void SetViewport(const Math::Viewport& viewport);

		void LookAt(const Math::Vector3& position, const Math::Vector3& target, const Math::Vector3& up);
		void SetLens(float nearPlane, float farPlane);
		void SetLens(float fovY, const Math::Viewport& viewport);
		void SetLens(float fovY, const Math::Viewport& viewport, float nearPlane, float farPlane);

		void SetView(const Math::Matrix& viewMatrix);

		void SetIsOrthographic(bool isOrthographic, float size)
		{
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

		DirectX::BoundingFrustum GetWorldSpaceFrustum() const;
	private:
		Math::Vector3 m_position;
		Math::Vector3 m_target;
		Math::Vector3 m_up;

		float m_nearPlane;
		float m_farPlane;
		float m_fovY;
		float m_size;
		Math::Viewport m_viewport;

		mutable Math::Matrix m_view;
		mutable Math::Matrix m_inverseView;
		mutable Math::Matrix m_projection;
		mutable Math::Matrix m_viewProjection;
		bool m_isOrthographic{ false };

		struct States
		{
			bool ViewDirty : 1;
			bool ProjectionDirty : 1;
			bool ViewProjectionDirty : 1;
		};
		mutable States m_states{};
	};
}