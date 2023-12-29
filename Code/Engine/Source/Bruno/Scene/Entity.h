#pragma once

#include <entt/entt.hpp>
//#include "Bruno/Scene/Scene.h"

namespace Bruno
{
	class Scene;

	class Entity
	{
	public:
		Entity(entt::entity handle, Scene* scene) : m_entityHandle(handle), m_scene(scene){}
		~Entity() {}

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args);

		template<typename T>
		T& GetComponent();

		template<typename T>
		const T& GetComponent() const;

		template<typename... T>
		bool HasComponent();

		template<typename... T>
		bool HasComponent() const;

	private:
		entt::entity m_entityHandle{ entt::null };
		Scene* m_scene{ nullptr };
	};

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
		return m_scene->m_registry.has<T...>(m_entityHandle);
	}

	template<typename... T>
	bool Entity::HasComponent() const
	{
		return m_scene->m_registry.has<T...>(m_entityHandle);
	}
}