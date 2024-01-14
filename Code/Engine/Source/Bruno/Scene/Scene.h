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

	enum class ActionMode
	{
		Add,
		Delete,
		Modify
	};
	using SceneHierarchyChangeCallback = std::function<void(Entity&, ActionMode)>;

	class Scene
	{
	public:
		Scene();

		Entity CreateEntity(const std::string& name = "Unnamed");
		Entity CreateEntity(Entity parent, const std::string& name);
		Entity InstantiateModel(std::shared_ptr<Model> model);

		template<typename... Components>
		auto GetAllEntitiesWith() { return m_registry.view<Components...>(); }

		Entity GetEntityWithUUID(UUID id) const;
		Math::Matrix GetWorldSpaceMatrix(Entity entity);
		Entity TryGetEntityWithUUID(UUID id) const;

		void OnUpdate(const GameTimer& timer, Camera& camera);

		void SetHierarchyChangeCallback(SceneHierarchyChangeCallback callback);

		friend class SceneRenderer;
		friend class ObjectSelector;
		friend class Entity;

	private:
		void CreateModelEntityHierarchy(Entity parent, std::shared_ptr<Model> model, const ModelNode& node);

		entt::registry m_registry;
		entt::entity m_sceneEntity{ entt::null };
		std::unordered_map<UUID, Entity> m_entityIdMap;

		SceneHierarchyChangeCallback m_hierarchyChangeCallback;

		struct ObjectBuffer
		{
			Math::Matrix World;
		};
		std::unique_ptr<ConstantBuffer<ObjectBuffer>> m_objectBuffer[Graphics::Core::FRAMES_IN_FLIGHT_COUNT];
	};
}

#include "Entity.inl"