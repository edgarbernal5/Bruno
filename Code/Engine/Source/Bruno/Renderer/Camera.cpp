#include "brpch.h"
#include "Camera.h"

namespace Bruno
{
	Camera::Camera() :
		m_position{},
		m_nearPlane{ 0.1f },
		m_farPlane{ 100.0f },
		m_fovY{ 100.0f },

		m_viewport{ 0, 0, 1, 1 }
	{
	}

	const Math::Matrix& Camera::GetView()
	{
		if (m_states.ViewDirty)
		{
			m_view = Math::Matrix::CreateLookAt(m_position, m_target, m_up);

			m_inverseView = m_view.Invert();
			m_states.ViewDirty = false;
		}
		return m_view;
	}

	const Math::Matrix& Camera::GetProjection()
	{
		if (m_states.ProjectionDirty)
		{
			m_projection = Math::Matrix::CreatePerspectiveFieldOfView(m_fovY, m_viewport.AspectRatio(), m_nearPlane, m_farPlane);
			m_states.ProjectionDirty = false;
		}
		return m_projection;
	}

	const Math::Matrix& Camera::GetViewProjection()
	{
		if (m_states.ViewProjectionDirty)
		{
			m_viewProjection = GetView() * GetProjection();
			m_states.ViewProjectionDirty = false;
		}
		return m_viewProjection;
	}

	void Camera::Rotate(const Math::Int2& mousePosition, const Math::Int2& previousPosition)
	{
		Math::Vector2 deltaAngles(2.0f * DirectX::XM_PI / m_viewport.width, DirectX::XM_PI / m_viewport.height);
		Math::Vector2 mouseVelocity((float)(mousePosition.x - previousPosition.x), (float)(mousePosition.y - previousPosition.y));
		auto angles = mouseVelocity * deltaAngles;

		auto zAxis = m_target - m_position;
		zAxis.Normalize();

		auto xAxis = m_up.Cross(zAxis);
		xAxis.Normalize();

		auto yAxis = zAxis.Cross(xAxis);
		yAxis.Normalize();

		xAxis = yAxis.Cross(zAxis);
		xAxis.Normalize();

		auto rotationMatrixPitch = Math::Matrix::CreateFromAxisAngle(xAxis, angles.y);
		auto position = Math::Vector3::Transform(m_position - m_target, rotationMatrixPitch) + m_target;

		auto rotationMatrixYaw = Math::Matrix::CreateFromAxisAngle(yAxis, angles.x);
		position = Math::Vector3::Transform(position - m_target, rotationMatrixYaw) + m_target;

		m_position = position;
		m_up = Math::Vector3::TransformNormal(yAxis, rotationMatrixPitch);
		m_up = Math::Vector3::TransformNormal(m_up, rotationMatrixYaw);
		m_states.ViewDirty = m_states.ViewProjectionDirty = true;
	}

	void Camera::SetViewport(const Math::Viewport& viewport)
	{
		m_viewport = viewport;
		m_states.ProjectionDirty = m_states.ViewProjectionDirty = true;
	}

	void Camera::HandTool(const Math::Int2& mousePosition, const Math::Int2& previousPosition)
	{
		Math::Vector2 mouseVelocity((float)(mousePosition.x - previousPosition.x), (float)(mousePosition.y - previousPosition.y));
		
		auto zAxis = m_target - m_position;
		zAxis.Normalize();

		auto xAxis = m_up.Cross(zAxis);
		xAxis.Normalize();

		auto deltaMovement = xAxis * mouseVelocity.x * 0.01f - m_up * mouseVelocity.y * 0.01f;
		m_position += deltaMovement;
		m_target += deltaMovement;

		m_states.ViewDirty = m_states.ViewProjectionDirty = true;
	}

	void Camera::PitchYaw(const Math::Int2& mousePosition, const Math::Int2& previousPosition)
	{
		Math::Vector2 deltaAngles(2.0f * DirectX::XM_PI / m_viewport.width, DirectX::XM_PI / m_viewport.height);
		Math::Vector2 mouseVelocity((float)(mousePosition.x - previousPosition.x), (float)(mousePosition.y - previousPosition.y));
		auto angles = mouseVelocity * deltaAngles;
		
		auto zAxis = m_target - m_position;
		float distance = zAxis.Length();
		zAxis.Normalize();

		auto xAxis = m_up.Cross(zAxis);
		xAxis.Normalize();

		auto yAxis = zAxis.Cross(xAxis);
		yAxis.Normalize();

		xAxis = yAxis.Cross(zAxis);
		xAxis.Normalize();

		auto rotationMatrixPitch = Math::Matrix::CreateFromAxisAngle(xAxis, angles.y);
		yAxis = Math::Vector3::TransformNormal(yAxis, rotationMatrixPitch);
		zAxis = Math::Vector3::TransformNormal(zAxis, rotationMatrixPitch);

		auto rotationMatrixYaw = Math::Matrix::CreateRotationY(angles.x);
		xAxis = Math::Vector3::TransformNormal(xAxis, rotationMatrixYaw);
		yAxis = Math::Vector3::TransformNormal(yAxis, rotationMatrixYaw);
		zAxis = Math::Vector3::TransformNormal(zAxis, rotationMatrixYaw);

		m_target = m_position + zAxis * distance;
		m_up = yAxis;

		m_states.ViewDirty = m_states.ViewProjectionDirty = true;
	}

	void Camera::Strafe(float delta)
	{
		auto zAxis = m_target - m_position;
		zAxis.Normalize();

		auto xAxis = m_up.Cross(zAxis);
		xAxis.Normalize();

		m_position += xAxis * delta;
		m_target += xAxis * delta;

		m_states.ViewDirty = m_states.ViewProjectionDirty = true;
	}

	void Camera::Walk(float delta)
	{
		auto zAxis = m_target - m_position;
		zAxis.Normalize();

		m_position += zAxis * delta;
		m_target += zAxis * delta;

		m_states.ViewDirty = m_states.ViewProjectionDirty = true;
	}

	void Camera::Zoom(float delta)
	{
		auto zAxis = m_target - m_position;
		zAxis.Normalize();
		auto newPosition = m_position + zAxis * delta;

		if (Math::Vector3::DistanceSquared(newPosition, m_target) < 0.25f)
			return;

		m_position = newPosition;
		m_states.ViewDirty = m_states.ViewProjectionDirty = true;
	}

	void Camera::LookAt(const Math::Vector3& position, const Math::Vector3& target, const Math::Vector3& up)
	{
		auto direction = target - position;
		auto zAxis = direction;
		zAxis.Normalize();
		
		auto xAxis = up.Cross(zAxis);
		xAxis.Normalize();

		auto yAxis = zAxis.Cross(xAxis);
		yAxis.Normalize();

		xAxis = yAxis.Cross(zAxis);
		xAxis.Normalize();

		m_position = position;
		m_target = target;
		m_up = yAxis;

		m_states.ViewDirty = m_states.ViewProjectionDirty = true;
	}
	
	void Camera::SetLens(float fovY, const Math::Viewport& viewport, float nearPlane, float farPlane)
	{
		m_fovY = fovY;
		m_viewport = viewport;
		m_nearPlane = nearPlane;
		m_farPlane = farPlane;
		m_states.ProjectionDirty = m_states.ViewProjectionDirty = true;
	}

	void Camera::UpdateMatrices()
	{
		if (m_states.ViewDirty)
		{
			m_view = Math::Matrix::CreateLookAt(m_position, m_target, m_up);
			
			m_inverseView = m_view.Invert();
			m_states.ViewDirty = false;
		}

		if (m_states.ProjectionDirty)
		{
			m_projection = Math::Matrix::CreatePerspectiveFieldOfView(m_fovY, m_viewport.AspectRatio(), m_nearPlane, m_farPlane);
			m_states.ProjectionDirty = false;
		}

		if (m_states.ViewProjectionDirty)
		{
			m_viewProjection = m_view * m_projection;
			m_states.ViewProjectionDirty = false;
		}
	}
}