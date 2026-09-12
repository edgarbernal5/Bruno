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
		Entity rootEntity = CreateEntity(L"Model_" + model->GetName());
		
		std::unordered_map<std::wstring, Entity> nodeEntityMap;
		
		CreateModelEntityHierarchy(rootEntity, model, model->GetRootNode(), nodeEntityMap);

		auto& lights = model->GetLights();
		
		if (lights.empty())
		{
			Entity lightParent = rootEntity;
			Entity lightEntity = CreateEntity(lightParent, L"Light_0");
        
			auto& transform = lightEntity.GetComponent<TransformComponent>();
			transform.Position = Math::Vector3 { 0.0f, 0.0f, 0.0f };
			transform.Rotation = Math::Quaternion::Identity;
			
			auto& comp = lightEntity.AddComponent<DirectionalLightComponent>();
			comp.Color = Math::Vector3 { 1.0f, 1.0f, 1.0f };
			comp.Intensity = 1.0f;
			comp.Direction = Math::Vector3(0.5f, -1.0f, 0.5f);
			comp.Direction.Normalize();
			
			return rootEntity;
		}
		
		for (const auto& light : lights)
		{
			// Si la luz viene amarrada a un nodo (FBX/GLTF), la colgamos de ese nodo.
			// Si no, la colgamos de la raíz del modelo.
			Entity lightParent = rootEntity;
			if (nodeEntityMap.find(light.NodeName) != nodeEntityMap.end())
			{
				lightParent = nodeEntityMap[light.NodeName];
			}

			Entity lightEntity = CreateEntity(lightParent, L"Light_" + light.NodeName);
        
			auto& transform = lightEntity.GetComponent<TransformComponent>();
			transform.Position = light.LocalPosition;
			transform.Rotation = light.LocalRotation; // Vital para Spot y Directional
			
			auto& comp = lightEntity.AddComponent<DirectionalLightComponent>();
			comp.Color = light.Color;
			comp.Intensity = light.Intensity;
			comp.Direction = light.Direction;
		}
		
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

	void Scene::CreateModelEntityHierarchy(Entity parent, std::shared_ptr<Model> model, const ModelNode& node, std::unordered_map<std::wstring, Entity>& nodeEntityMap)
	{
		const auto& nodes = model->GetNodes();

		if (node.IsRoot() && node.Meshes.size() == 0)
		{
			for (uint32_t child : node.Children)
			{
				CreateModelEntityHierarchy(parent, model, nodes[child], nodeEntityMap);
			}

			return;
		}
		
		Entity nodeEntity = CreateEntity(parent, node.Name);
		nodeEntity.GetComponent<TransformComponent>().ApplyTransform(node.LocalTransform);

		nodeEntityMap[node.Name] = nodeEntity;
		
		auto& meshes = model->GetMeshes();
		auto& materials = model->GetMaterials();

		for (size_t i = 0; i < node.Meshes.size(); i++)
		{
			uint32_t submeshIndex = node.Meshes[i]; 
        
			// Si hay más de una malla, creamos sub-entidades. Si es solo una, reusamos el nodo actual.
			Entity targetEntity = (node.Meshes.size() == 1) ? nodeEntity : CreateEntity(nodeEntity, node.Name + L"_Submesh" + std::to_wstring(i));
        
			auto& modelComponent = targetEntity.AddComponent<ModelComponent>(model->GetHandle(), submeshIndex);
			auto& boundingBoxComponent = targetEntity.AddComponent<BoundingBoxComponent>();

			auto& mesh = meshes[submeshIndex];
			auto bbox = mesh->GetBoundingBox();
			boundingBoxComponent.Center = bbox.Center;
			boundingBoxComponent.Extents = bbox.Extents;
        
			for (size_t j = 0; j < materials.size(); ++j)
			{
				modelComponent.Materials->SetMaterial(static_cast<uint32_t>(j), materials[j]->GetHandle());
			}
		}
		
		for (uint32_t child : node.Children)
		{
			CreateModelEntityHierarchy(nodeEntity, model, nodes[child], nodeEntityMap);
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
