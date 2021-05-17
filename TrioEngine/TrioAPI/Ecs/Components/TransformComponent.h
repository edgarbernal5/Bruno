#pragma once

#include "Math/MathVector.h"

namespace TrioEngine
{
	struct TransformComponent
	{
		enum FLAGS
		{
			EMPTY = 0,
			DIRTY = 1 << 0,
		};
		uint32_t _flags = DIRTY;

		Vector3 m_localScale;
		Vector3 m_localPosition;
		Quaternion m_localRotation;

		Matrix m_world;

		TransformComponent() {
			m_localScale = Vector3::One;
			m_localPosition = Vector3::Zero;
			m_localRotation = Quaternion::Identity;

			m_world = Matrix::Identity;
		}

		void Update() {
			if (IsDirty())
			{
				SetDirty(false);

				m_world = GetLocalMatrix();
			}
		}

		void Reset() {
			m_localScale = Vector3::One;
			m_localPosition = Vector3::Zero;
			m_localRotation = Quaternion::Identity;

			m_world = Matrix::Identity;
		}

		inline void SetDirty(bool value = true)
		{
			if (value) { _flags |= DIRTY; }
			else { _flags &= ~DIRTY; }
		}

		inline bool IsDirty() const
		{
			return _flags & DIRTY; 
		}

	private:
		Matrix GetLocalMatrix() const
		{
			/*XMVECTOR S_local = XMLoadFloat3(&scale_local);
			XMVECTOR R_local = XMLoadFloat4(&rotation_local);
			XMVECTOR T_local = XMLoadFloat3(&translation_local);
			return
				XMMatrixScalingFromVector(S_local) *
				XMMatrixRotationQuaternion(R_local) *
				XMMatrixTranslationFromVector(T_local);

				*/

			//return Matrix::CreateScale(m_localScale) *
			//	Matrix::CreateFromQuaternion(m_localRotation) *
			//	Matrix::CreateTranslation(m_localPosition);

			return Matrix::CreateTranslation(m_localPosition)  *
				Matrix::CreateFromQuaternion(m_localRotation) *
				Matrix::CreateScale(m_localScale);
		}
	};
}