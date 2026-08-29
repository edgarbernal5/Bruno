#pragma once

#include "Entity.h"

#include <vector>
#include "Bruno/Platform/DirectX/ConstantBuffer.h"
#include "Bruno/Renderer/Camera.h"

namespace Bruno
{
	class Camera;
	class GameTimer;
	class Model;
	struct ModelNode;

	struct SceneObjectBuffer
	{
		Math::Matrix WorldViewProjection;
	};

	class Scene
	{
	public:
		Scene();

		Entity CreateEntity(const std::wstring& name = L"Unnamed");
		Entity CreateEntity(Entity parent, const std::wstring& name);
		Entity InstantiateModel(std::shared_ptr<Model> model);

		template<typename... Components>
		auto Clear() { return m_registry.clear<Components...>(); }
		
		template<typename... Components>
		auto GetAllEntitiesWith() { return m_registry.view<Components...>(); }
		
		template<typename... Components>
		auto TryGetWith(Entity entity) { return m_registry.try_get<Components...>(entity.GetEntityHandle()); }
		
		Entity GetEntityWithUUID(UUID id);
		Math::Matrix GetLocalSpaceMatrix(Entity entity);
		Math::Matrix GetWorldSpaceMatrix(Entity entity);
		Entity TryGetEntityWithUUID(UUID id);

		void OnUpdate(const GameTimer& timer, Camera& camera);
		
		template<typename Component>
		auto OnComponentUpdated() 
		{
			// EnTT permite crear un sink a partir de una señal (sigh)
			return entt::sink{ m_registry.on_update<Component>() };
		}
		
		template<typename Component>
		auto OnConstruct()
		{
			return entt::sink{ m_registry.on_construct<Component>() };
		}
		
		template<typename Component>
		auto OnDestroy()
		{
			return entt::sink{ m_registry.on_destroy<Component>() };
		}
		
		friend class SceneRenderer;
		friend class ObjectSelector;
		friend class Entity;

	private:
		void CreateModelEntityHierarchy(Entity parent, std::shared_ptr<Model> model, const ModelNode& node);
		void SortEntities();

		entt::registry m_registry;
		entt::entity m_sceneEntity{ entt::null };
		std::unordered_map<UUID, entt::entity> m_entityIdMap;
	};
}

#include "Entity.inl"