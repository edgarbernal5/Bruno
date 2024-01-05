#include "brpch.h"
#include "Entity.h"

#include "Scene.h"

namespace Bruno
{
	Entity::operator bool() const
	{
		return (m_entityHandle != entt::null) && m_scene && m_scene->m_registry.valid(m_entityHandle);
	}

	Entity Entity::GetParent() const
	{

		return {};
	}

	void Entity::SetParent(Entity parent)
	{

	}
}