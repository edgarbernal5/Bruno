#include "brpch.h"
#include "Scene.h"
#include "Systems/HierarchySystem.h"

namespace Bruno
{
	Entity::operator bool() const
	{
		return (m_entityHandle != entt::null) && m_scene && m_scene->m_registry.valid(m_entityHandle);
	}

	UUID Entity::GetUUID() const
	{
		return GetComponent<IdComponent>().Id; 
	}

	Entity Entity::GetParent() const
	{
		auto* hierarchy = m_scene->TryGetWith<HierarchyComponent>(*this);
		if (hierarchy && hierarchy->Parent != entt::null)
		{
			return Entity{ hierarchy->Parent, m_scene };
		}
		return Entity{ entt::null, m_scene };
	}

	void Entity::SetParent(Entity parent)
	{
		entt::entity parentHandle = parent ? parent.GetEntityHandle() : entt::null;
    
		// El sistema se encarga de todo: desvincular, validar ciclos, enlazar O(1)
		//HierarchySystem::SetParent(m_scene->GetRegistry(), m_entityHandle, parentHandle);
	}
}
