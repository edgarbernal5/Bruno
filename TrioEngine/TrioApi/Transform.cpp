#include "stdafx.h"
#include "Transform.h"

#include "GameObject.h"

namespace TrioEngine
{
	Transform::Transform() :
		m_localPosition(0.0f, 0.0f, 0.0f),
		m_localScale(1.0f, 1.0f, 1.0f),
		m_position(m_localPosition),
		m_rotation(m_localRotation),
		m_scale(m_localScale),
		m_dirty(false)
	{

	}

	Transform::~Transform()
	{

	}

	void Transform::SetParent(const std::shared_ptr<Transform>& parent)
	{
		Vector3 position = GetPosition();
		Quaternion rotation = GetRotation();
		Vector3 scale = GetScale();

		auto oldParent = m_parent.lock();
		if (oldParent)
		{
			for (int i = 0; i < oldParent->GetChildCount(); ++i)
			{
				if (oldParent->GetChild(i).get() == this)
				{
					oldParent->m_children.erase(oldParent->m_children.begin() + i);
					break;
				}
			}
			m_parent.reset();
		}

		if (parent)
		{
			parent->m_children.push_back(GetGameObject()->GetTransform());
			m_parent = parent;
		}

		SetPosition(position);
		SetRotation(rotation);
		SetScale(scale);
	}

	std::shared_ptr<Transform> Transform::GetRoot() const
	{
		auto parent = GetParent();
		if (parent)
		{
			return parent->GetRoot();
		}
		else
		{
			return GetGameObject()->GetTransform();
		}
	}

	void Transform::SetLocalPosition(const Vector3& position)
	{
		m_localPosition = position;

		MarkDirty();
	}

	void Transform::SetLocalRotation(const Quaternion& rotation)
	{
		m_localRotation = rotation;

		MarkDirty();
	}

	void Transform::SetLocalScale(const Vector3& scale)
	{
		m_localScale = scale;

		MarkDirty();
	}

	const Vector3& Transform::GetPosition()
	{
		UpdateMatrix();

		return m_position;
	}

	void Transform::SetPosition(const Vector3& position)
	{
		Vector3 localPosition = position;

		auto parent = m_parent.lock();
		if (parent)
		{
			localPosition = Vector3::Transform(position, parent->GetWorldToLocalMatrix());
		}

		SetLocalPosition(localPosition);
	}

	const Quaternion& Transform::GetRotation()
	{
		UpdateMatrix();

		return m_rotation;
	}

	void Transform::SetRotation(const Quaternion& rotation)
	{
		Quaternion localRotation = rotation;

		auto parent = m_parent.lock();
		if (parent)
		{
			Quaternion invRotation;
			rotation.Inverse(invRotation);

			localRotation = invRotation * rotation;
		}

		SetLocalRotation(localRotation);
	}

	const Vector3& Transform::GetScale()
	{
		UpdateMatrix();

		return m_scale;
	}

	void Transform::SetScale(const Vector3& scale)
	{
		Vector3 localScale = scale;

		auto parent = m_parent.lock();
		if (parent)
		{
			const auto& parentScale = parent->GetScale();
			localScale = Vector3(scale.x / parentScale.x, scale.y / parentScale.y, scale.z / parentScale.z);
		}

		SetLocalScale(localScale);
	}

	const Matrix& Transform::GetLocalToWorldMatrix()
	{
		UpdateMatrix();

		return m_localToWorld;
	}

	const Matrix& Transform::GetWorldToLocalMatrix()
	{
		UpdateMatrix();

		return m_worldToLocal;
	}

	Vector3 Transform::GetRight()
	{
		UpdateMatrix();

		return m_localToWorld.Right();
	}

	Vector3 Transform::GetUp()
	{
		UpdateMatrix();

		return m_localToWorld.Up();
	}

	Vector3 Transform::GetForward()
	{
		UpdateMatrix();

		return m_localToWorld.Forward();
	}

	void Transform::MarkDirty()
	{
		m_dirty = true;
		GetGameObject()->OnTransformDirty();

		for (auto& child : m_children)
		{
			child->MarkDirty();
		}
	}

	void Transform::UpdateMatrix()
	{
		if (m_dirty)
		{
			m_dirty = false;

			auto parent = m_parent.lock();
			if (parent)
			{
				m_localToWorld = parent->GetLocalToWorldMatrix() * Matrix::CreateTRS(m_localPosition, m_localRotation, m_localScale);
				m_position = Vector3::Transform(m_localPosition, parent->GetLocalToWorldMatrix());
				m_rotation = parent->GetRotation() * m_localRotation;
				const auto& parentScale = parent->GetScale();
				m_scale = Vector3(parentScale.x * m_localScale.x, parentScale.y * m_localScale.y, parentScale.z * m_localScale.z);
			}
			else
			{
				m_localToWorld = Matrix::CreateTRS(m_localPosition, m_localRotation, m_localScale);
				m_position = m_localPosition;
				m_rotation = m_localRotation;
				m_scale = m_localScale;
			}

			m_localToWorld.Invert(m_worldToLocal);
		}
	}
}