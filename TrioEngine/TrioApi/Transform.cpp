#include "stdafx.h"
#include "Transform.h"

#include "GameObject.h"

namespace TrioEngine
{
	Transform::Transform() :
		m_localPosition(0, 0, 0),
		m_localScale(1, 1, 1),
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

		auto old_parent = m_parent.lock();
		if (old_parent)
		{
			for (int i = 0; i < old_parent->GetChildCount(); ++i)
			{
				if (old_parent->GetChild(i).get() == this)
				{
					old_parent->m_children.erase(old_parent->m_children.begin() + i);
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
		auto parent = this->GetParent();
		if (parent)
		{
			return parent->GetRoot();
		}
		else
		{
			return this->GetGameObject()->GetTransform();
		}
	}

	void Transform::SetLocalPosition(const Vector3& position)
	{
		m_localPosition = position;

		this->MarkDirty();
	}

	void Transform::SetLocalRotation(const Quaternion& rotation)
	{
		m_localRotation = rotation;

		this->MarkDirty();
	}

	void Transform::SetLocalScale(const Vector3& scale)
	{
		m_localScale = scale;

		this->MarkDirty();
	}

	const Vector3& Transform::GetPosition()
	{
		this->UpdateMatrix();

		return m_position;
	}

	void Transform::SetPosition(const Vector3& pos)
	{
		Vector3 local_position;

		auto parent = m_parent.lock();
		if (parent)
		{
			//local_position = parent->GetWorldToLocalMatrix().MultiplyPoint3x4(pos);
		}
		else
		{
			local_position = pos;
		}

		this->SetLocalPosition(local_position);
	}

	const Quaternion& Transform::GetRotation()
	{
		this->UpdateMatrix();

		return m_rotation;
	}

	void Transform::SetRotation(const Quaternion& rotation)
	{
		Quaternion local_rotation;

		auto parent = m_parent.lock();
		if (parent)
		{
			//local_rotation = Quaternion::Inverse(parent->GetRotation()) * rotation;
		}
		else
		{
			local_rotation = rotation;
		}

		this->SetLocalRotation(local_rotation);
	}

	const Vector3& Transform::GetScale()
	{
		this->UpdateMatrix();

		return m_scale;
	}

	void Transform::SetScale(const Vector3& scale)
	{
		Vector3 local_scale;

		auto parent = m_parent.lock();
		if (parent)
		{
			const auto& parent_scale = parent->GetScale();
			local_scale = Vector3(scale.x / parent_scale.x, scale.y / parent_scale.y, scale.z / parent_scale.z);
		}
		else
		{
			local_scale = scale;
		}

		this->SetLocalScale(local_scale);
	}

	const Matrix& Transform::GetLocalToWorldMatrix()
	{
		this->UpdateMatrix();

		return m_local_to_world;
	}

	const Matrix& Transform::GetWorldToLocalMatrix()
	{
		this->UpdateMatrix();

		return m_world_to_local;
	}

	Vector3 Transform::GetRight()
	{
		this->UpdateMatrix();

		return m_local_to_world.Left();
		//return m_local_to_world.MultiplyDirection(Vector3(1, 0, 0));
	}

	Vector3 Transform::GetUp()
	{
		this->UpdateMatrix();

		return m_local_to_world.Up();
		//return m_local_to_world.MultiplyDirection(Vector3(0, 1, 0));
	}

	Vector3 Transform::GetForward()
	{
		this->UpdateMatrix();

		return m_local_to_world.Forward();
		//return m_local_to_world. * Vector3(0, 0, 1);
	}

	void Transform::MarkDirty()
	{
		m_dirty = true;
		GetGameObject()->OnTransformDirty();

		for (auto& i : m_children)
		{
			i->MarkDirty();
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

			}
			else
			{
				m_position = m_localPosition;
				m_rotation = m_localRotation;
				m_scale = m_localScale;

				//m_world_to_local = Matrix::CreateWorld
			}
		}
	}
}