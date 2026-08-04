#include "brpch.h"
#include "Camera.h"

namespace Bruno
{
	Camera::Camera() :
		m_position{},
		m_nearPlane{ 0.1f },
		m_farPlane{ 100.0f },
		m_fovY{ 45.0f },
		m_size{ 1.0f },

		m_viewport{ 0, 0, 1, 1 }
	{
	}

	const Math::Matrix& Camera::GetView() const
	{
		if (m_states.ViewDirty)
		{
			m_view = Math::Matrix::CreateLookAt(m_position, m_target, m_up);

			m_inverseView = m_view.Invert();
			m_states.ViewDirty = false;
		}
		return m_view;
	}

	const Math::Matrix& Camera::GetInverseView() const
	{
		if (m_states.ViewDirty)
		{
			m_view = Math::Matrix::CreateLookAt(m_position, m_target, m_up);

			m_inverseView = m_view.Invert();
			m_states.ViewDirty = false;
		}
		return m_inverseView;
	}

	const Math::Matrix& Camera::GetProjection() const
	{
		if (m_states.ProjectionDirty)
		{
			if (m_isOrthographic)
			{
				float aspectRatio = m_viewport.width / m_viewport.height;
				float halfHeight = m_size * 0.5f;
				float halfWidth = halfHeight * aspectRatio;
				m_projection = Math::Matrix::CreateOrthographicOffCenter(-halfWidth, halfWidth, -halfHeight, halfHeight, m_nearPlane, m_farPlane);
			}
			else
			{
				m_projection = Math::Matrix::CreatePerspectiveFieldOfView(m_fovY, m_viewport.AspectRatio(), m_nearPlane, m_farPlane);
			}
			
			m_states.ProjectionDirty = false;
		}
		return m_projection;
	}

	const Math::Matrix& Camera::GetViewProjection() const
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
		// 1. Calculamos el delta del mouse
		Math::Vector2 deltaAngles(2.0f * DirectX::XM_PI / m_viewport.width, DirectX::XM_PI / m_viewport.height);
		Math::Vector2 mouseVelocity(static_cast<float>(mousePosition.x - previousPosition.x), static_cast<float>(mousePosition.y - previousPosition.y));
		auto angles = mouseVelocity * deltaAngles;

		// EL SECRETO #1: Eje Y Global (World Up) constante.
		// Esto evita que la cámara se incline hacia los lados (Roll)
		Math::Vector3 worldUp = Math::Vector3::Up;

		// Vector de dirección que va del Target hacia la Cámara (el radio de la órbita)
		Math::Vector3 dir = m_position - m_target;

		// 2. YAW (Rotación Horizontal)
		// Rotamos SIEMPRE alrededor del Eje Y GLOBAL. 
		auto rotationMatrixYaw = Math::Matrix::CreateFromAxisAngle(worldUp, -angles.x);
		dir = Math::Vector3::Transform(dir, rotationMatrixYaw);

		// 3. PITCH (Rotación Vertical)
		// Calculamos el Eje Right (X local) cruzando el World Up con nuestra dirección actual
		Math::Vector3 right = worldUp.Cross(dir); 
		right.Normalize();

		// Rotamos la dirección alrededor del eje Right local
		auto rotationMatrixPitch = Math::Matrix::CreateFromAxisAngle(right, angles.y);
		Math::Vector3 newDir = Math::Vector3::Transform(dir, rotationMatrixPitch);

		// EL SECRETO #2: Prevenir el "Gimbal Lock" (Pasar por encima del polo)
		// Normalizamos la nueva dirección temporalmente solo para medir su ángulo
		Math::Vector3 dirNormalized = newDir;
		dirNormalized.Normalize();
    
		// El Producto Punto nos dirá qué tan paralelos estamos al WorldUp
		// 1.0f es mirando totalmente arriba, -1.0f es mirando totalmente abajo
		float dot = dirNormalized.Dot(worldUp);
    
		// Si NO estamos demasiado cerca del cenit o el nadir (los polos), aceptamos el Pitch.
		// 0.99f equivale a unos 8 grados de límite. Evita que la cámara tiemble o se voltee.
		if (std::abs(dot) < 0.99f)
		{
			dir = newDir;
		}

		// 4. Aplicamos los resultados
		m_position = m_target + dir;
    
		// EL SECRETO #3: Fijar el Up al World Up.
		// Al pasarle el World Up puro a Math::Matrix::CreateLookAt en GetView(), 
		// la matriz se encargará matemáticamente de calcular el Up Local perfecto y ortogonal.
		m_up = worldUp; 

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

	void Camera::SetView(const Math::Matrix& viewMatrix)
	{
		m_view = viewMatrix;

		m_inverseView = m_view.Invert();
		m_states.ViewDirty = false;
		m_states.ViewProjectionDirty = true;
	}

	void Camera::SetLens(float nearPlane, float farPlane)
	{
		m_nearPlane = nearPlane;
		m_farPlane = farPlane;
		m_states.ProjectionDirty = m_states.ViewProjectionDirty = true;
	}

	void Camera::SetLens(float fovY, const Math::Viewport& viewport)
	{
		m_fovY = fovY;
		m_viewport = viewport;
		m_states.ProjectionDirty = m_states.ViewProjectionDirty = true;
	}
}