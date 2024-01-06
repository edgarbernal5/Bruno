#pragma once

#include "Entity.h"

#include <vector>
#include "Bruno/Platform/DirectX/ConstantBuffer.h"
#include <Bruno/Renderer/Camera.h>

namespace Bruno
{
	class Camera;
	class GameTimer;
	class Model;
	struct ModelNode;
	struct RenderItem;

	struct Transformable {
		Math::Vector3 Position;
		Math::Quaternion Rotation;
		Math::Vector3 Scale;
		Math::Matrix WorldTransform;

		Transformable() :Scale(Math::Vector3::One * 1.0f) {}
	};

	class Scene
	{
	public:
		Scene(Camera& camera);

		Entity CreateEntity(const std::string& name = "Empty-name");
		Entity CreateEntity(Entity parent, const std::string& name);
		void InstantiateModel(std::shared_ptr<Model> model);
		const std::vector<std::shared_ptr<RenderItem>>& GetRenderItems() { return m_renderItems; }
		void OnUpdate(const GameTimer& timer);

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_registry.view<Components...>();
		}

		Entity TryGetEntityWithUUID(UUID id) const;

		friend class SceneRenderer;
		friend class ObjectSelector;
		friend class Entity;

	private:
		void CreateModelEntityHierarchy(Entity parent, std::shared_ptr<Model> model, const ModelNode& node);

		entt::registry m_registry;
		entt::entity m_sceneEntity{ entt::null };
		std::unordered_map<UUID, Entity> m_entityIdMap;

		Camera& m_camera;
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