#pragma once

#include "TrioApiRequisites.h"
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

		bool GetEnabled() { return m_enabled; }
		std::shared_ptr<GameObject> GetGameObject() const { return m_ownerObject.lock(); }
		const std::shared_ptr<Transform>& GetTransform() const;

		void SetEnabled(bool enabled) { m_enabled = enabled; }

		friend class GameObject;
	protected:
		virtual void Update() { }
		virtual void OnTransformDirty() { }
		
	private:
		bool m_enabled;

		std::weak_ptr<GameObject> m_ownerObject;
	};
}
