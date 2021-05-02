#pragma once

#include "TrioApiRequisites.h"

#include "Math/MathVector.h"

namespace TrioEngine
{
	class TRIO_API_EXPORT Camera
	{
	public:
		inline const Vector3& GetPosition() {
			return m_position;
		}
		
		inline const Vector3& GetTarget() {
			return m_target;
		}

		inline const Vector3& GetUp() {
			return m_up;
		}

		inline float GetNearPlane() {
			return m_nearPlane;
		}

		inline float GetFarPlane() {
			return m_farPlane;
		}

		inline float GetFieldOfView() {
			return m_fieldOfView;
		}

		inline float GetAspectRatio() {
			return m_aspectRatio;
		}

		inline void SetAspectRatio(float ratio) {
			m_aspectRatio = ratio;
		}

	private:
		Vector3 m_position = { 5.0f, 5.0f, 5.0f };
		Vector3 m_target;
		Vector3 m_up = { 0.0f, 1.0f, 0.0f };

		float m_nearPlane = 0.5f;
		float m_farPlane = 1000.0f;
		float m_fieldOfView = 1.5708f;
		float m_aspectRatio = 1.0f;
	};
}