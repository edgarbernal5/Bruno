#include "brpch.h"
#include "Scene.h"

#include "Components.h"

#include "Bruno/Renderer/Model.h"
#include "Bruno/Renderer/Material.h"
#include <Bruno/Core/GameTimer.h>

#include "Bruno/Scene/Entity.h"
#include "Systems/HierarchySystem.h"

namespace Bruno
{
	Scene::Scene()
	{
		m_sceneEntity = m_registry.create();
	}

	Entity Scene::CreateEntity(const std::wstring& name)
	{
		return CreateEntity({}, name);
	}

	Entity Scene::CreateEntity(Entity parent, const std::wstring& name)
	{
		entt::entity rawHandle = m_registry.create();
		auto entity = Entity{ rawHandle, this };
		
		auto& idComponent = entity.AddComponent<IdComponent>();
		idComponent.Id = {};

		entity.AddComponent<TransformComponent>();
		entity.AddComponent<NameComponent>().Name = name;

		if (parent)
		{
			HierarchySystem::SetParent(m_registry, rawHandle, parent.GetEntityHandle());
		}

		m_entityIdMap[idComponent.Id] = rawHandle;

		//SortEntities();
		return entity;
	}

	Entity Scene::InstantiateModel(std::shared_ptr<Model> model)
	{
		Entity rootEntity = CreateEntity(L"Mesh test");
		CreateModelEntityHierarchy(rootEntity, model, model->GetRootNode());

		return rootEntity;
	}

	void Scene::OnUpdate(const GameTimer& timer, Camera& camera)
	{
		
	}

	Math::Matrix Scene::GetLocalSpaceMatrix(Entity entity)
	{
		return entity.GetComponent<TransformComponent>().LocalTransform;
	}

	Math::Matrix Scene::GetWorldSpaceMatrix(Entity entity)
	{
		return entity.GetComponent<TransformComponent>().WorldTransform;
	}

	Entity Scene::GetEntityWithUUID(UUID id)
	{
		auto it = m_entityIdMap.find(id);
		
		BR_ASSERT(it != m_entityIdMap.end(), "Invalid entity Id");
    
		return Entity{ it->second, this };
	}

	Entity Scene::TryGetEntityWithUUID(UUID id)
	{
		auto it = m_entityIdMap.find(id);
		if (it != m_entityIdMap.end())
		{
			return Entity{ it->second, this };
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

		auto& meshes = model->GetMeshes();
		if (node.Meshes.size() == 1)
		{
			uint32_t submeshIndex = node.Meshes[0];
			auto& modelComponent = nodeEntity.AddComponent<ModelComponent>(model->GetHandle(), submeshIndex);
			auto& boundingBoxComponent = nodeEntity.AddComponent<BoundingBoxComponent>();

			auto& mesh = meshes[submeshIndex];
			auto bbox = mesh->GetBoundingBox();
			boundingBoxComponent.Center = bbox.Center;
			boundingBoxComponent.Extents = bbox.Extents;
			
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
				auto& boundingBoxComponent = childEntity.AddComponent<BoundingBoxComponent>();

				auto& mesh = meshes[submeshIndex];
				auto bbox = mesh->GetBoundingBox();
				boundingBoxComponent.Center = bbox.Center;
				boundingBoxComponent.Extents = bbox.Extents;
				
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
