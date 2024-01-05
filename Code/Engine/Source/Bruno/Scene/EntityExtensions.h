#pragma once

namespace Bruno
{
	template<typename T, typename... Args>
	T& Entity::AddComponent(Args&&... args)
	{
		BR_ASSERT(!HasComponent<T>(), "Entity already has component!");
		return m_scene->m_registry.emplace<T>(m_entityHandle, std::forward<Args>(args)...);
	}

	template<typename T>
	T& Entity::GetComponent()
	{
		BR_ASSERT(HasComponent<T>(), "Entity doesn't have component!");
		return m_scene->m_registry.get<T>(m_entityHandle);
	}

	template<typename T>
	const T& Entity::GetComponent() const
	{
		BR_ASSERT(HasComponent<T>(), "Entity doesn't have component!");
		return m_scene->m_registry.get<T>(m_entityHandle);
	}

	template<typename... T>
	bool Entity::HasComponent()
	{
		return m_scene->m_registry.all_of<T...>(m_entityHandle);
	}

	template<typename... T>
	bool Entity::HasComponent() const
	{
		return m_scene->m_registry.all_of<T...>(m_entityHandle);
	}
}