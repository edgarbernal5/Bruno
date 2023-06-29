#pragma once

#include <Bruno/Math/Math.h>

namespace Bruno
{
	class Camera
	{
	public:
		Camera();

		//const Math::Vector3& GetPosition() const { return m_position; }
		//const Math::Vector3& GetTarget() const { return m_target; }
		//const Math::Vector3& GetUp() const { return m_up; }

		const Math::Matrix& GetView() const { return m_view; }
		const Math::Matrix& GetViewProjection() const { return m_viewProjection; }

		//void SetPosition(const Math::Vector3& position);
		//void SetTarget(const Math::Vector3& target);
		//void SetUp(const Math::Vector3& up);

		void SetViewport(const Math::Viewport& viewport);

		void LookAt(const Math::Vector3& position, const Math::Vector3& target, const Math::Vector3& up);
		void SetLens(float fovY, const Math::Viewport& viewport, float nearPlane, float farPlane);

		void Rotate(const Math::Int2& mousePosition, const Math::Int2& previousPosition);
		void Zoom(float delta);

		void UpdateMatrices();

	private:
		Math::Vector3 m_position;
		//Math::Vector3 m_target;
		//Math::Vector3 m_up;

		float m_zoom;
		float m_nearPlane;
		float m_farPlane;
		float m_fovY;
		Math::Viewport m_viewport;

		Math::Matrix m_view;
		Math::Matrix m_inverseView;
		Math::Matrix m_projection;
		Math::Matrix m_viewProjection;
		Math::Quaternion m_targetQRotation;
		Math::Quaternion m_currentQRotation;

		struct States
		{
			bool ViewDirty : 1;
			bool ProjectionDirty : 1;
			bool ViewProjectionDirty : 1;
		};
		States m_states{};

		Math::Vector2 GetScreenCoordToNDC(const Math::Int2& mousePosition);
		Math::Quaternion GetNDCToArcBall(const Math::Vector2& point);
	};
}