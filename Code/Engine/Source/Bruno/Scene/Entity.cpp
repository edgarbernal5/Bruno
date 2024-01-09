#include "brpch.h"
#include "Scene.h"

namespace Bruno
{
	Entity::operator bool() const
	{
		return (m_entityHandle != entt::null) && m_scene && m_scene->m_registry.valid(m_entityHandle);
	}

	std::vector<UUID>& Entity::GetChildren()
	{
		return GetComponent<HierarchyComponent>().Children;
	}

	UUID Entity::GetUUID() const
	{
		return GetComponent<IdComponent>().Id; 
	}

	Entity Entity::GetParent() const
	{
		return m_scene->TryGetEntityWithUUID(GetParentUUID());
	}

	UUID Entity::GetParentUUID() const
	{
		return GetComponent<HierarchyComponent>().Parent;
	}

	void Entity::SetParentUUID(UUID parent)
	{
		GetComponent<HierarchyComponent>().Parent = parent;
	}

	bool Entity::RemoveChild(Entity child)
	{
		UUID childId = child.GetUUID(); 
		std::vector<UUID>& children = GetChildren();
		auto it = std::find(children.begin(), children.end(), childId);
		if (it != children.end())
		{
			children.erase(it);
			return true;
		}

		return false;
	}

	void Entity::SetParent(Entity parent)
	{
		Entity currentParent = GetParent();
		if (currentParent == parent)
		{
			return;
		}

		if (currentParent)
		{
			currentParent.RemoveChild(*this);
		}
		SetParentUUID(parent.GetUUID());

		if (parent)
		{
			auto& parentChildren = parent.GetChildren();
			UUID uuid = GetUUID();
			if (std::find(parentChildren.begin(), parentChildren.end(), uuid) == parentChildren.end())
			{
				parentChildren.emplace_back(uuid);
			}
		}
	}
}