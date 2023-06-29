#include "brpch.h"
#include "Camera.h"


namespace Bruno
{
	Camera::Camera()
	{
	}

	void Camera::SetPosition(const Math::Vector3& position)
	{
		m_position = position;
		m_states.ViewDirty = m_states.ViewProjectionDirty = true;
	}

	void Camera::Rotate(Math::Int2 mousePosition, Math::Int2 previousPosition)
	{
		auto mousePositionNDC = GetScreenCoordToNDC(mousePosition);
		auto currentQRotation = GetNDCToArcBall(mousePositionNDC);
		
		auto previousmousePositionNDC = GetScreenCoordToNDC(previousPosition);
		auto previousQRotation = GetNDCToArcBall(previousmousePositionNDC);

		m_targetQRotation = (m_targetQRotation * previousQRotation * currentQRotation);
		m_targetQRotation.Normalize();

		m_currentQRotation = Math::Quaternion::Slerp(m_currentQRotation, m_targetQRotation, 0.9f);
		m_states.ViewDirty = m_states.ViewProjectionDirty = true;
	}

	void Camera::SetTarget(const Math::Vector3& target)
	{
		m_target = target;
		m_states.ViewDirty = m_states.ViewProjectionDirty = true;
	}

	void Camera::SetUp(const Math::Vector3& up)
	{
		m_up = up;
		m_states.ViewDirty = m_states.ViewProjectionDirty = true;
	}

	Math::Vector2 Camera::GetScreenCoordToNDC(const Math::Int2& mousePosition)
	{
		return 
		{
			1.0f - mousePosition.x * 2.0f / m_viewport.width,
				2.0f * mousePosition.y / m_viewport.height - 1.0f
		};
	}

	Math::Quaternion Camera::GetNDCToArcBall(const Math::Vector2& point)
	{
		const float dist = point.Dot(point);

		if (dist <= 1.0f)
		{
			return {
				point.x, point.y, std::sqrtf(1.0f - dist), 0.0f
			};
		}
		Math::Vector2 proj = point;
		proj.Normalize();

		return {
			proj.x, proj.y, 0.0f, 0.0f
		};
	}

	void Camera::SetViewport(const Math::Viewport& viewport)
	{
		m_viewport = viewport;
		m_states.ProjectionDirty = m_states.ViewProjectionDirty = true;
	}

	void Camera::LookAt(const Math::Vector3& position, const Math::Vector3& target, const Math::Vector3& up)
	{
		m_position = position;
		m_target = target;
		m_up = up;

		auto direction = target - position;
		auto zAxis = direction;
		zAxis.Normalize();
		
		auto xAxis = up.Cross(zAxis);
		xAxis.Normalize();

		auto yAxis = zAxis.Cross(xAxis);
		yAxis.Normalize();

		xAxis = yAxis.Cross(zAxis);
		xAxis.Normalize();

		auto rotationMatrix = Math::Matrix{
			xAxis, yAxis, zAxis
		};

		m_targetQRotation = Math::Quaternion::CreateFromRotationMatrix(rotationMatrix);
		m_targetQRotation.Normalize();

		m_currentQRotation = m_targetQRotation;
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
			//m_view = Math::Matrix::CreateLookAt(m_position, m_target, m_up);
			auto rotationMatrix = Math::Matrix::CreateFromQuaternion(m_currentQRotation);
			m_view = rotationMatrix * Math::Matrix::CreateTranslation(-m_position);
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