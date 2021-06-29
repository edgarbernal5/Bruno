#pragma once

#include "Math/MathVector.h"

namespace BrunoEngine
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
			SetDirty(true);
		}

		void UpdateTransformParented(const TransformComponent& parent) {
			Matrix localMatrix = GetLocalMatrix();
			Matrix localParentMatrix = parent.m_world;
			localMatrix = localMatrix * localParentMatrix;

			m_world = localMatrix;
		}

		inline void SetDirty(bool value)
		{
			if (value) { _flags |= DIRTY; }
			else { _flags &= ~DIRTY; }
		}

		inline bool IsDirty() const
		{
			return _flags & DIRTY; 
		}

		void ApplyTransform()
		{
			SetDirty(true);

			m_world.Decompose(m_localScale, m_localRotation, m_localPosition);
		}

		void MatrixTransform(const Matrix& matrix) {
			SetDirty(true);

			Vector3 scale, translation;
			Quaternion rotation;
			Matrix LM = GetLocalMatrix() * matrix;
			LM.Decompose(scale, rotation, translation);

			m_localScale = scale;
			m_localRotation = rotation;
			m_localPosition = translation;
		}

		void Translate(const Vector3& delta) {
			m_localPosition += delta;
			SetDirty(true);
		}

		void Scale(const Vector3& delta) {
			m_localScale += delta;
			SetDirty(true);
		}

		void ScaleUniform(float scaleFactor) {
			m_localScale *= scaleFactor;
			SetDirty(true);
		}

		void Rotate(const Quaternion& delta) {
			Quaternion localRot = m_localRotation * delta;
			localRot.Normalize();

			m_localRotation = localRot;
			SetDirty(true);
		}

		void RotatePitchYawRoll(const Vector3& delta) {
			Quaternion localRot = m_localRotation;
			Quaternion x = Quaternion::CreateFromYawPitchRoll(0, delta.x, 0);
			Quaternion y = Quaternion::CreateFromYawPitchRoll(delta.y, 0, 0);
			Quaternion z = Quaternion::CreateFromYawPitchRoll(0, 0, delta.z);

			localRot = x * localRot;
			localRot = localRot * y;
			localRot = z * localRot;

			localRot.Normalize();

			m_localRotation = localRot;
			SetDirty(true);
		}

		void SetLocalPosition(const Vector3& position) {
			m_localPosition = position;
			SetDirty(true);
		}

		void SetLocalRotation(const Quaternion& rotation) {
			m_localRotation = rotation;
			m_localRotation.Normalize();

			SetDirty(true);
		}

		void SetLocalScale(const Vector3& scale) {
			m_localScale = scale;
			SetDirty(true);
		}

	private:
		Matrix GetLocalMatrix() const
		{
			return Matrix::CreateScale(m_localScale) *
				Matrix::CreateFromQuaternion(m_localRotation) *
				Matrix::CreateTranslation(m_localPosition);
		}
	};
}