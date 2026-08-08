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

	const Math::Matrix& Camera::GetViewInverse() const
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

	void Camera::SetTarget(const Math::Vector3& target)
	{
		m_target = target;
		m_states.ViewDirty = m_states.ViewProjectionDirty = true;
	}

	void Camera::SetPosition(const Math::Vector3& position)
	{
		m_position = position;
		m_states.ViewDirty = m_states.ViewProjectionDirty = true;
	}

	void Camera::SetViewport(const Math::Viewport& viewport)
	{
		m_viewport = viewport;
		m_states.ProjectionDirty = m_states.ViewProjectionDirty = true;
	}

	void Camera::LookAt(const Math::Vector3& position, const Math::Vector3& target, const Math::Vector3& up)
	{
		Math::Vector3 zAxis = target - position;
		if (zAxis.LengthSquared() < 0.00001f)
		{
			return;
		}
		zAxis.Normalize();

		Math::Vector3 safeUp = up;
		if (safeUp.LengthSquared() < 0.00001f) safeUp = Math::Vector3(0.0f, 1.0f, 0.0f);

		// Si zAxis (mirada) y safeUp son paralelos
		if (std::abs(zAxis.Dot(safeUp)) > 0.999f)
		{
			// Elegimos un up perpendicular de rescate
			safeUp = (std::abs(zAxis.y) > 0.99f) ? Math::Vector3(0.0f, 0.0f, 1.0f) : Math::Vector3(0.0f, 1.0f, 0.0f);
		}

		// Reconstruimos la base (Left-Handed)
		Math::Vector3 xAxis = safeUp.Cross(zAxis);
		xAxis.Normalize();

		Math::Vector3 yAxis = zAxis.Cross(xAxis);
		yAxis.Normalize();

		m_position = position;
		m_target = target;
		m_up = yAxis; // Guardamos un Up perfecto, garantizado 100% de no ser paralelo a zAxis

		m_states.ViewDirty = m_states.ViewProjectionDirty = true;
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

	void Camera::HandTool(const Math::Int2& mousePosition, const Math::Int2& previousPosition)
	{
		Math::Vector2 mouseVelocity(static_cast<float>(mousePosition.x - previousPosition.x), static_cast<float>(mousePosition.y - previousPosition.y));
		
		auto zAxis = m_target - m_position;
		zAxis.Normalize();

		auto xAxis = m_up.Cross(zAxis);
		xAxis.Normalize();

		auto deltaMovement = xAxis * mouseVelocity.x * 0.01f - m_up * mouseVelocity.y * 0.01f;
		m_position += deltaMovement;
		m_target += deltaMovement;

		m_states.ViewDirty = m_states.ViewProjectionDirty = true;
	}

	void Camera::Rotate(const Math::Int2& mousePosition, const Math::Int2& previousPosition)
	{
		Math::Vector2 deltaAngles(2.0f * DirectX::XM_PI / m_viewport.width, DirectX::XM_PI / m_viewport.height);
		Math::Vector2 mouseVelocity(static_cast<float>(mousePosition.x - previousPosition.x), static_cast<float>(mousePosition.y - previousPosition.y));
		auto angles = mouseVelocity * deltaAngles;

		Math::Vector3 worldUp = Math::Vector3(0.0f, 1.0f, 0.0f);
		Math::Vector3 forward = m_target - m_position;
		float distance = forward.Length();

		if (distance < 0.0001f)
		{
			return;
		}
		forward /= distance;

		// 1. Calcular Right
		Math::Vector3 right = worldUp.Cross(forward);
    
		// Si la cámara mira recto abajo/arriba
		if (right.LengthSquared() < 0.00001f)
		{
			right = m_up.Cross(forward);
			if (right.LengthSquared() < 0.00001f) 
				right = Math::Vector3(1.0f, 0.0f, 0.0f);
		}
		right.Normalize();

		// 2. YAW 
		auto rotationMatrixYaw = Math::Matrix::CreateFromAxisAngle(worldUp, -angles.x);
		forward = Math::Vector3::Transform(forward, rotationMatrixYaw);
		right = Math::Vector3::Transform(right, rotationMatrixYaw);

		// 3. PITCH
		auto rotationMatrixPitch = Math::Matrix::CreateFromAxisAngle(right, angles.y);
		Math::Vector3 newForward = Math::Vector3::Transform(forward, rotationMatrixPitch);

		// 4. PREVENCIÓN DE GIMBAL LOCK ¡CON ESCAPE AUTOMÁTICO!
		float currentDot = forward.Dot(worldUp);
		float newDot = newForward.Dot(worldUp);

		// Permitimos la rotación si:
		// a) Estamos en una zona segura (ej. < 0.99f, a unos 8 grados de los polos)
		// b) O si la rotación nos está ALEJANDO del polo (|newDot| < |currentDot|)
		if (std::abs(newDot) < 0.99f || std::abs(newDot) < std::abs(currentDot))
		{
			forward = newForward;
		}

		// 5. Aplicar Estado
		m_up = forward.Cross(right);
		m_up.Normalize();

		m_position = m_target - (forward * distance);
		m_states.ViewDirty = m_states.ViewProjectionDirty = true;
	}
	
	void Camera::PitchYaw(const Math::Int2& mousePosition, const Math::Int2& previousPosition)
	{
		Math::Vector2 deltaAngles(2.0f * DirectX::XM_PI / m_viewport.width, DirectX::XM_PI / m_viewport.height);
		Math::Vector2 mouseVelocity(static_cast<float>(mousePosition.x - previousPosition.x), static_cast<float>(mousePosition.y - previousPosition.y));
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

	void Camera::Zoom(float delta)
	{
		auto zAxis = m_target - m_position;
		zAxis.Normalize();
		auto newPosition = m_position + zAxis * delta;

		if (Math::Vector3::DistanceSquared(newPosition, m_target) < 0.25f)
		{
			return;
		}

		m_position = newPosition;
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

	DirectX::BoundingFrustum Camera::GetWorldSpaceFrustum() const
	{
		DirectX::BoundingFrustum frustum;
    
		// 1. Carga segura de la Proyección (Evita corrupción de memoria/SIMD)
		// Casteamos tu matriz a XMFLOAT4X4 para que DirectXMath la asimile correctamente
		const DirectX::XMFLOAT4X4* projFloat = reinterpret_cast<const DirectX::XMFLOAT4X4*>(&GetProjection());
		DirectX::XMMATRIX xmProjection = DirectX::XMLoadFloat4x4(projFloat);
    
		// Crear el frustum en espacio local
		DirectX::BoundingFrustum::CreateFromMatrix(frustum, xmProjection);

		// 2. Carga segura de la Matriz de Vista Inversa
		// ¡TRUCO!: Es más seguro invertir la matriz usando tu propia librería de Math
		// antes de pasarla a DirectXMath. Si tu clase Matrix tiene un método .Invert(), úsalo:
		Math::Matrix viewInverse = GetView().Invert(); 
    
		// Si tu Math::Matrix NO tiene Invert(), usa este bloque en su lugar:
		/*
		const DirectX::XMFLOAT4X4* viewFloat = reinterpret_cast<const DirectX::XMFLOAT4X4*>(&view);
		DirectX::XMMATRIX xmView = DirectX::XMLoadFloat4x4(viewFloat);
		DirectX::XMVECTOR det;
		DirectX::XMMATRIX xmViewInverse = DirectX::XMMatrixInverse(&det, xmView);
		*/

		// Cargar la matriz inversa segura a XMMATRIX
		const DirectX::XMFLOAT4X4* viewInvFloat = reinterpret_cast<const DirectX::XMFLOAT4X4*>(&viewInverse);
		DirectX::XMMATRIX xmViewInverse = DirectX::XMLoadFloat4x4(viewInvFloat);

		// 3. Transformar al Mundo
		frustum.Transform(frustum, xmViewInverse);

		return frustum;
	}
}
