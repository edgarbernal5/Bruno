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
	struct RenderItem;

	struct SceneObjectBuffer
	{
		Math::Matrix World;
	};

	class Scene
	{
	public:
		Scene();

		Entity CreateEntity(const std::wstring& name = L"Unnamed");
		Entity CreateEntity(Entity parent, const std::wstring& name);
		Entity InstantiateModel(std::shared_ptr<Model> model);

		template<typename... Components>
		auto GetAllEntitiesWith() { return m_registry.view<Components...>(); }

		Entity GetEntityWithUUID(UUID id) const;
		Math::Matrix GetLocalSpaceMatrix(Entity entity);
		Math::Matrix GetWorldSpaceMatrix(Entity entity);
		Entity TryGetEntityWithUUID(UUID id) const;

		void OnUpdate(const GameTimer& timer, Camera& camera);
		
		template<typename Component>
		auto OnComponentUpdated() 
		{
			// EnTT permite crear un sink a partir de una señal (sigh)
			return entt::sink{ m_registry.on_update<Component>() };
		}
		
		friend class SceneRenderer;
		friend class ObjectSelector;
		friend class Entity;

	private:
		void CreateModelEntityHierarchy(Entity parent, std::shared_ptr<Model> model, const ModelNode& node);
		void SortEntities();

		entt::registry m_registry;
		entt::entity m_sceneEntity{ entt::null };
		std::unordered_map<UUID, Entity> m_entityIdMap;

		std::unique_ptr<ConstantBuffer<SceneObjectBuffer>> m_objectBuffer[Graphics::Core::FRAMES_IN_FLIGHT_COUNT];
	};
}

#include "Entity.inl"