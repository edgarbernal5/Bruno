#include "brpch.h"
#include "Scene.h"

#include "Components.h"

#include "Bruno/Renderer/Model.h"
#include "Bruno/Renderer/Material.h"
#include <Bruno/Renderer/RenderItem.h>
#include <Bruno/Core/GameTimer.h>

#include "Bruno/Scene/Entity.h"

namespace Bruno
{
	Scene::Scene()
	{
		m_sceneEntity = m_registry.create();

		for (size_t i = 0; i < Graphics::Core::FRAMES_IN_FLIGHT_COUNT; i++)
		{
			m_objectBuffer[i] = std::make_unique<ConstantBuffer<SceneObjectBuffer>>(Graphics::Core::SCENE_OBJECT_COUNT);
		}
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntity({}, name);
	}

	Entity Scene::CreateEntity(Entity parent, const std::string& name)
	{
		auto entity = Entity{ m_registry.create(), this };
		auto& idComponent = entity.AddComponent<IdComponent>();
		idComponent.Id = {};

		entity.AddComponent<TransformComponent>();
		entity.AddComponent<HierarchyComponent>();
		entity.AddComponent<NameComponent>().Name = name;

		if (parent)
		{
			entity.SetParent(parent);
		}

		m_entityIdMap[idComponent.Id] = entity;

		//SortEntities();
		return entity;
	}

	Entity Scene::InstantiateModel(std::shared_ptr<Model> model)
	{
		Entity rootEntity = CreateEntity("Mesh test");
		CreateModelEntityHierarchy(rootEntity, model, model->GetRootNode());

		return rootEntity;
	}

	void Scene::OnUpdate(const GameTimer& timer, Camera& camera)
	{
		auto device = Graphics::GetDevice();
		uint32_t frameIndex = device->GetFrameId();
		auto entities = GetAllEntitiesWith<IdComponent, TransformComponent>();
		uint32_t index = 0;
		for (auto& ent : entities) {
			auto [idComponent, transformComponent] = entities.get<IdComponent, TransformComponent>(ent);

			auto world = GetWorldSpaceMatrix(GetEntityWithUUID(idComponent.Id));

			Math::Matrix mvpMatrix = world * camera.GetViewProjection();

			SceneObjectBuffer objectBuffer;
			objectBuffer.World = mvpMatrix;

			m_objectBuffer[frameIndex]->SetMappedData(index, objectBuffer);
			index++;
		}
	}

	Math::Matrix Scene::GetLocalSpaceMatrix(Entity entity)
	{
		return entity.GetComponent<TransformComponent>().GetTransform();
	}

	Math::Matrix Scene::GetWorldSpaceMatrix(Entity entity)
	{
		Math::Matrix transform;

		Entity parent = TryGetEntityWithUUID(entity.GetParentUUID());
		if (parent) {
			transform = GetWorldSpaceMatrix(parent);
		}
		return transform * entity.GetComponent<TransformComponent>().GetTransform();
	}

	Entity Scene::GetEntityWithUUID(UUID id) const
	{
		BR_ASSERT(m_entityIdMap.find(id) != m_entityIdMap.end(), "Invalid entity Id");
		return m_entityIdMap.at(id);
	}

	Entity Scene::TryGetEntityWithUUID(UUID id) const
	{
		auto it = m_entityIdMap.find(id);
		if (it != m_entityIdMap.end())
		{
			return it->second;
		}

		return Entity{};
	}

	void Scene::CreateModelEntityHierarchy(Entity parent, std::shared_ptr<Model> model, const ModelNode& node)
	{
		const auto& nodes = model->GetNodes();

		if (node.IsRoot() && node.Meshes.size() == 0)
		{
			for (uint32_t child : node.Children)
			{
				CreateModelEntityHierarchy(parent, model, nodes[child]);
			}

			return;
		}
		Entity nodeEntity = CreateEntity(parent, node.Name);
		nodeEntity.GetComponent<TransformComponent>().ApplyTransform(node.LocalTransform);

		if (node.Meshes.size() == 1)
		{
			uint32_t submeshIndex = node.Meshes[0];
			auto& modelComponent = nodeEntity.AddComponent<ModelComponent>(model->GetHandle(), submeshIndex);

			for (size_t j = 0; j < model->GetMaterials().size(); ++j)
			{
				auto& material = model->GetMaterials()[j];
				modelComponent.Materials->SetMaterial(j, material->GetHandle());
			}
		}
		else if (node.Meshes.size() > 1)
		{
			for (size_t i = 0; i < node.Meshes.size(); i++)
			{
				uint32_t submeshIndex = node.Meshes[i]; 

				Entity childEntity = CreateEntity(nodeEntity, node.Name);
				auto& modelComponent = childEntity.AddComponent<ModelComponent>(model->GetHandle(), submeshIndex);

				for (size_t j = 0; j < model->GetMaterials().size(); ++j)
				{
					auto& material = model->GetMaterials()[j];
					modelComponent.Materials->SetMaterial(j, material->GetHandle());
				}
			}
		}

		for (uint32_t child : node.Children)
		{
			CreateModelEntityHierarchy(nodeEntity, model, nodes[child]);
		}
	}

	void Scene::SortEntities()
	{
		m_registry.sort<IdComponent>([&](const auto lhs, const auto rhs)
		{
			auto lhsEntity = m_entityIdMap.find(lhs.Id);
			auto rhsEntity = m_entityIdMap.find(rhs.Id);
			return static_cast<uint32_t>(lhsEntity->second) < static_cast<uint32_t>(rhsEntity->second);
		});
	}
}