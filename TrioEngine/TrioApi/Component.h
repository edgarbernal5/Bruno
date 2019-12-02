#pragma once

#include "TrioApiDecl.h"
#include "Object.h"
#include <string>

namespace TrioEngine
{
	class GameObject;
	class Transform;

	class Component : public Object
	{
	public:
		Component();
		virtual ~Component();
		//std::shared_ptr<GameObject> GetGameObject() const { return m_object.lock(); }

		GameObject* GetGameObject() const
		{
			return m_object;
		}
		const std::shared_ptr<Transform>& GetTransform() const;

	protected:
		virtual void Update() { }
		virtual void LateUpdate() { }
		virtual void OnTransformDirty() { }

	private:
		friend class GameObject;

	private:
		//std::weak_ptr<GameObject> m_object;
		GameObject* m_object;
	};
}
