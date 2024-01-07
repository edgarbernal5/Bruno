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

	struct Transformable
	{
		Math::Vector3 Position;
		Math::Quaternion Rotation;
		Math::Vector3 Scale;
		Math::Matrix WorldTransform;

		Transformable() :Scale(Math::Vector3::One * 1.0f) {}
	};

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
		auto GetAllEntitiesWith()
		{
			return m_registry.view<Components...>();
		}

		Entity TryGetEntityWithUUID(UUID id) const;

		const std::vector<std::shared_ptr<RenderItem>>& GetRenderItems() { return m_renderItems; }
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
		
		std::vector<std::shared_ptr<Model>>			m_models;
		std::vector<std::shared_ptr<RenderItem>>	m_renderItems;

		struct ObjectBuffer
		{
			Math::Matrix World;
		};
		std::unique_ptr<ConstantBuffer<ObjectBuffer>> m_objectBuffer[Graphics::Core::FRAMES_IN_FLIGHT_COUNT];

		std::vector<std::shared_ptr<Transformable>>	m_transformableObjects;
	};
}

#include "Entity.inl"