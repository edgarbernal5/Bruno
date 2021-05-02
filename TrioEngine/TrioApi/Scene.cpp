#include "stdafx.h"
#include "Scene.h"

#include "Graphics/Models/Model.h"
#include "Graphics/Models/ModelMesh.h"
#include "Graphics/Models/ModelMeshPart.h"
#include "Graphics/Models/Material.h"

#include <unordered_set>

namespace TrioEngine
{
	Scene* Scene::g_activeScene = nullptr;
	Camera* Scene::g_mainCamera = nullptr;

	Scene::Scene()
	{
		g_mainCamera = new Camera();
	}

	Scene::~Scene()
	{
		if (g_mainCamera != nullptr) {
			delete g_mainCamera;
			g_mainCamera = nullptr;
		}
	}

	Scene* Scene::GetActiveScene()
	{
		return g_activeScene;
	}

	void Scene::SetActiveScene(Scene* scene)
	{
		g_activeScene = scene;
	}

	Camera* Scene::GetCamera()
	{
		return g_mainCamera;
	}

	Entity Scene::CreateMaterialEntity(std::string name)
	{
		Entity entity = CreateEntity();
		NameComponent& nameComponent = m_names.Create(entity);
		nameComponent.m_name = name;

		m_materials.Create(entity);

		return entity;
	}

	Entity Scene::CreateMesh(std::string name)
	{
		Entity entity = CreateEntity();

		NameComponent& nameComponent = m_names.Create(entity);
		nameComponent.m_name = name;

		m_meshes.Create(entity);

		return entity;
	}

	void Scene::LoadFromModel(Model* model)
	{
		m_names.Clear();
		m_transforms.Clear();
		m_hierarchy.Clear();
		m_meshes.Clear();
		m_materials.Clear();

		std::unordered_set<Material*> allMaterials;
		std::unordered_map<Material*, Entity> materialIndexes;

		for (auto modelMesh : model->GetModelMeshes()) {
			for (auto meshPart : modelMesh->GetModelMeshParts()) {
				if (meshPart->GetMaterial()) {
					if (allMaterials.find(meshPart->GetMaterial()) == allMaterials.end()) {
						allMaterials.insert(meshPart->GetMaterial());
					}
				}
			}
		}

		for (auto modelMaterial : allMaterials) {
			Entity materialEntity = CreateMaterialEntity(modelMaterial->GetName());
			MaterialComponent& material = *m_materials.GetComponent(materialEntity);

			const Vector3& diffuse = modelMaterial->GetDiffuseColor();
			material.baseColor = Vector4(diffuse.x, diffuse.y, diffuse.z, 1.0f);

			materialIndexes[modelMaterial] = materialEntity;
		}

		int meshIndex = 0;
		for (auto modelMesh : model->GetModelMeshes()) {
			meshIndex++;

			std::stringstream ss;
			ss << "mesh_" << meshIndex;
			Entity meshEntity = CreateMesh(ss.str());

			MeshComponent& mesh = *m_meshes.GetComponent(meshEntity);

			for (auto meshPart : modelMesh->GetModelMeshParts()) {
				mesh.m_subMeshes.push_back(MeshComponent::SubMesh());

				MeshComponent::SubMesh& subMesh = mesh.m_subMeshes.back();
				subMesh.m_startIndex = meshPart->GetStartIndex();
				subMesh.m_vertexOffset = meshPart->GetVertexOffset();
				subMesh.m_primitiveCount = meshPart->GetPrimitiveCount();
				subMesh.m_vertexCount = meshPart->GetVertexCount();

				mesh.m_vertexBuffer = *meshPart->GetVertexBuffer();
				mesh.m_indexBuffer = *meshPart->GetIndexBuffer();

				subMesh.m_materialId = materialIndexes[meshPart->GetMaterial()];
			}
		}
	}

	void Scene::OnWindowSizeChanged(int width, int height) {
		float aspectRatio = (float)width / (float)height;
		g_mainCamera->SetAspectRatio(aspectRatio);
	}

	void Scene::Update()
	{
		for (size_t i = 0; i < m_transforms.GetCount(); i++)
		{
			TransformComponent& transform = m_transforms[i];
			transform.Update();
		}
	}
}