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
		Entity(entt::entity handle, Scene* scene) : m_entityHandle(handle), m_scene(scene){}
		~Entity() {}

		operator bool() const;
		
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

		std::vector<UUID>& GetChildren();
		UUID GetUUID() const;
		Entity GetParent() const;
		UUID GetParentUUID() const;
		void SetParentUUID(UUID parent);
		bool RemoveChild(Entity child);
		void SetParent(Entity parent);
	private:
		entt::entity m_entityHandle{ entt::null };
		Scene* m_scene{ nullptr };
	};

}