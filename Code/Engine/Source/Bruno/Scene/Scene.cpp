#include "brpch.h"
#include "Scene.h"

#include "Components.h"

#include "Bruno/Renderer/Model.h"
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
			m_objectBuffer[i] = std::make_unique<ConstantBuffer<ObjectBuffer>>();
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

		return entity;
	}

	Entity Scene::InstantiateModel(std::shared_ptr<Model> model)
	{
		Entity rootEntity = CreateEntity("Mesh test");
		CreateModelEntityHierarchy(rootEntity, model, model->GetRootNode());

		if (m_hierarchyChangeCallback)
		{
			m_hierarchyChangeCallback(rootEntity, ActionMode::Add);
		}

		auto& meshes = model->GetMeshes();
		for (auto& mesh : meshes)
		{
			auto boxRenderItem = std::make_shared<RenderItem>();
			boxRenderItem->IndexCount = mesh->GetIndexCount();
			boxRenderItem->BaseVertexLocation = mesh->GetBaseVertex();
			boxRenderItem->StartIndexLocation = mesh->GetBaseIndex();
			boxRenderItem->Material = model->GetMaterial(mesh->GetMaterialIndex()).get();
			boxRenderItem->IndexBuffer = model->GetIndexBuffer();
			boxRenderItem->VertexBuffer = model->GetVertexBuffer();
			m_renderItems.push_back(boxRenderItem);
		}
		m_transformableObjects.push_back(std::make_shared<Transformable>());

		m_models.push_back(model);

		return rootEntity;
	}

	void Scene::OnUpdate(const GameTimer& timer, Camera& camera)
	{
		auto device = Graphics::GetDevice();
		auto& objectSelected = m_transformableObjects[0];
		auto world = Math::Matrix::CreateScale(objectSelected->Scale) * Math::Matrix::CreateFromQuaternion(objectSelected->Rotation) * Math::Matrix::CreateTranslation(objectSelected->Position);
		objectSelected->WorldTransform = world;

		Math::Matrix mvpMatrix = world * camera.GetViewProjection();

		ObjectBuffer objectBuffer;
		objectBuffer.World = mvpMatrix;

		uint32_t frameIndex = device->GetFrameId();
		m_objectBuffer[frameIndex]->SetMappedData(objectBuffer);
	}

	void Scene::SetHierarchyChangeCallback(SceneHierarchyChangeCallback callback)
	{
		m_hierarchyChangeCallback = callback;
	}

	Entity Scene::TryGetEntityWithUUID(UUID id) const
	{
		auto it = m_entityIdMap.find(id);
		if (it != m_entityIdMap.end())
			return it->second;

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
		nodeEntity.GetComponent<TransformComponent>().SetTransform(node.LocalTransform);

		if (node.Meshes.size() == 1)
		{
			uint32_t submeshIndex = node.Meshes[0];
			auto& mc = nodeEntity.AddComponent<ModelComponent>(model->Handle(), submeshIndex);
		}
		else if (node.Meshes.size() > 1)
		{
			for (size_t i = 0; i < node.Meshes.size(); i++)
			{
				uint32_t submeshIndex = node.Meshes[i]; 

				Entity childEntity = CreateEntity(nodeEntity, node.Name);
				childEntity.AddComponent<ModelComponent>(model->Handle(), submeshIndex);
			}
		}

		for (uint32_t child : node.Children)
		{
			CreateModelEntityHierarchy(nodeEntity, model, nodes[child]);
		}
	}
}