#pragma once

#include "Components.h"
#include <entt/entt.hpp>

namespace Bruno
{
	class Scene;

	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene) : m_entityHandle(handle), m_scene(scene) { }
		~Entity() = default;

		operator uint32_t () const { return static_cast<uint32_t>(m_entityHandle); }
		operator bool() const;

		bool operator==(const Entity& other) const
		{
			return m_entityHandle == other.m_entityHandle && m_scene == other.m_scene;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}
		
		template<typename T, typename... Args>
		decltype(auto) AddComponent(Args&&... args);

		template<typename T, typename... Args>
		decltype(auto) AddOrReplaceComponent(Args&&... args);

		template<typename T>
		T& GetComponent();

		template<typename T>
		const T& GetComponent() const;

		template<typename... T>
		bool HasComponent();

		template<typename... T>
		bool HasComponent() const;

		template<typename T, typename... Func>
		void Patch(Func &&...func) const;
		
		
		UUID GetUUID() const;
		Entity GetParent() const;
		void SetParent(Entity parent);

		entt::entity GetEntityHandle() const { return m_entityHandle; }
	private:
		entt::entity m_entityHandle{ entt::null };
		Scene* m_scene{ nullptr };
	};
}