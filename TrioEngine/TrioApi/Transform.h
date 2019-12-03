#pragma once

#include "TrioApiRequisites.h"

#include "Component.h"
#include "Math\MathVector.h"
#include <string>

namespace TrioEngine
{
	class Transform : public Component
	{
	public:
		Transform();
		virtual ~Transform();

		const std::shared_ptr<Transform>& GetChild(int index) const
		{
			return m_children[index];
		}

		int GetChildCount() const
		{
			return m_children.size();
		}

		const Vector3& GetLocalPosition() const
		{
			return m_localPosition;
		}

		const Quaternion& GetLocalRotation() const
		{
			return m_localRotation;
		}

		const Vector3& GetLocalScale() const
		{
			return m_localScale;
		}

		std::shared_ptr<Transform> GetParent() const
		{
			return m_parent.lock();
		}
		
		const Vector3& GetPosition();

		std::shared_ptr<Transform> GetRoot() const;

		const Vector3& GetScale();

		void SetParent(const std::shared_ptr<Transform>& parent);

		const Quaternion& GetRotation();
		
		void SetLocalPosition(const Vector3& position);
		void SetLocalRotation(const Quaternion& rotation);
		void SetLocalScale(const Vector3& scale);
		
		void SetPosition(const Vector3& pos);
		void SetRotation(const Quaternion& rotation);
		void SetScale(const Vector3& scale);
		
		const Matrix& GetLocalToWorldMatrix();
		const Matrix& GetWorldToLocalMatrix();
		
		Vector3 GetRight();
		Vector3 GetUp();
		Vector3 GetForward();

	private:
		void MarkDirty();
		void UpdateMatrix();

		std::weak_ptr<Transform> m_parent;
		std::vector<std::shared_ptr<Transform>> m_children;

		Vector3 m_localPosition;
		Quaternion m_localRotation;
		Vector3 m_localScale;

		Vector3 m_position;
		Quaternion m_rotation;
		Vector3 m_scale;
		
		Matrix m_local_to_world;
		Matrix m_world_to_local;

		bool m_dirty;
	};
}
