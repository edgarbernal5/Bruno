#include "stdafx.h"
#include "Scene.h"

#include "Graphics/Models/Model.h"
#include "Graphics/Models/ModelMesh.h"
#include "Graphics/Models/ModelBone.h"
#include "Graphics/Models/ModelMeshPart.h"
#include "Graphics/Models/Material.h"
#include "Utils/TextureLoader.h"

#include <unordered_set>

namespace BrunoEngine
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

	void Scene::UpdateCamera(const Camera& camera) {
		*g_mainCamera = camera;
	}

	Entity Scene::CreateEntityForMaterial(std::string name)
	{
		Entity entity = CreateEntity();
		NameComponent& nameComponent = m_names.Create(entity);
		nameComponent.m_name = name;

		m_materials.Create(entity);

		return entity;
	}

	Entity Scene::CreateEntityForMesh(std::string name)
	{
		Entity entity = CreateEntity();

		NameComponent& nameComponent = m_names.Create(entity);
		nameComponent.m_name = name;

		m_transforms.Create(entity);
		m_meshes.Create(entity);

		return entity;
	}

	Entity Scene::CreateEntityForBone(std::string name)
	{
		Entity entity = CreateEntity();

		NameComponent& nameComponent = m_names.Create(entity);
		nameComponent.m_name = name;

		m_transforms.Create(entity);

		return entity;
	}

	void Scene::LoadFromModel(Model* model)
	{
		ResetEntities();
		m_names.Clear();
		m_transforms.Clear();
		m_hierarchies.Clear();
		m_meshes.Clear();
		m_materials.Clear();
		m_boundingBoxes.Clear();

		std::unordered_set<Material*> allMaterials;
		std::unordered_map<Material*, Entity> materialIndexes;
		std::unordered_map<ModelBone*, Entity> boneIndexes;
		int boneIndex = 0;
		for (auto modelBone : model->GetModelBones()) {
			boneIndex++;

			std::stringstream ss;
			ss << "bone_" << boneIndex << "_" << modelBone->GetName();

			Entity boneEntity = CreateEntityForBone(ss.str());
			TransformComponent& transform = *m_transforms.GetComponent(boneEntity);

			boneIndexes[modelBone] = boneEntity;

			Vector3 scale(1.0f), position;
			Quaternion rotation = Quaternion::Identity;

			bool valid = modelBone->GetTransform().Decompose(scale, rotation, position);
			if (valid) {
				transform.m_localScale = scale;
				transform.m_localRotation = rotation;
				transform.m_localPosition = position;
			}
			transform.SetDirty(true);
			transform.Update();

			ModelBone* parentBone = modelBone->GetParentBone();
			if (parentBone != nullptr) {
				auto parentEntity = boneIndexes.find(parentBone);
				if (parentEntity != boneIndexes.end()) {
					ComponentAttach(boneEntity, parentEntity->second, true);
				}
			}
		}

		for (auto modelMesh : model->GetModelMeshes()) {
			for (auto meshPart : modelMesh->GetModelMeshParts()) {
				if (meshPart->GetMaterial()) {
					if (allMaterials.find(meshPart->GetMaterial()) == allMaterials.end()) {
						allMaterials.insert(meshPart->GetMaterial());
					}
				}
			}
		}
		int materialIndex = 0;
		for (auto modelMaterial : allMaterials) {
			materialIndex++;

			std::stringstream ss;
			ss << "material_" << materialIndex << "_" << modelMaterial->GetName();

			Entity materialEntity = CreateEntityForMaterial(ss.str());
			MaterialComponent& material = *m_materials.GetComponent(materialEntity);

			const Vector3& diffuse = modelMaterial->GetDiffuseColor();
			material.baseColor = Vector4(diffuse.x, diffuse.y, diffuse.z, 1.0f);

			Texture* diffuseTexture = modelMaterial->GetTextureByName("Texture");
			material.diffuseTexture = reinterpret_cast<Texture2D*>(diffuseTexture);

			materialIndexes[modelMaterial] = materialEntity;
		}

		int meshIndex = 0;
		for (auto modelMesh : model->GetModelMeshes()) {
			meshIndex++;

			std::stringstream ss;
			ss << "mesh_" << meshIndex << "_" << modelMesh->GetName();

			Entity meshEntity = boneIndexes[modelMesh->GetParentBone()];
			
			MeshComponent& mesh = m_meshes.Create(meshEntity);
			NameComponent& name = *m_names.GetComponent(meshEntity);
			name.m_name = ss.str();

			BoundingBoxComponent& box = m_boundingBoxes.Create(meshEntity);
			BoundingBox bbox;
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

				BoundingBox::CreateMerged(bbox, bbox, modelMesh->GetBoundingBox());
			}

			box.m_center = bbox.Center;
			box.m_extents = bbox.Extents;
		}
	}

	void Scene::ComponentAttach(Entity entity, Entity parent, bool child_already_in_local_space)
	{
		if (m_hierarchies.Contains(entity)) {
			ComponentDetach(entity);
		}

		m_hierarchies.Create(entity).m_parentId = parent;

		if (m_hierarchies.GetCount() > 1) {
			for (size_t i = m_hierarchies.GetCount() - 1; i > 0; --i)
			{
				Entity parentCandidateEntity = m_hierarchies.GetEntity(i);
				for (size_t j = 0; j < i; ++j)
				{
					const HierarchyComponent& childCandidate = m_hierarchies[j];

					if (childCandidate.m_parentId == parentCandidateEntity)
					{
						m_hierarchies.MoveItem(i, j);
						++i; // next outer iteration will check the same index again as parent candidate, however things were moved upwards, so it will be a different entity!
						break;
					}
				}
			}
		}
		HierarchyComponent& parentcomponent = *m_hierarchies.GetComponent(entity);

		TransformComponent* transformParent = m_transforms.GetComponent(parent);
		if (transformParent == nullptr)
		{
			transformParent = &m_transforms.Create(parent);
		}

		TransformComponent* transformChild = m_transforms.GetComponent(entity);
		if (transformChild == nullptr)
		{
			transformChild = &m_transforms.Create(entity);
			transformParent = m_transforms.GetComponent(parent); // after transforms.Create(), transform_parent pointer could have become invalidated!
		}
		if (!child_already_in_local_space)
		{
			Matrix B = transformParent->m_world.Invert();
			transformChild->MatrixTransform(B);
			transformChild->Update();
		}
		transformChild->UpdateTransformParented(*transformParent);
	}

	void Scene::ComponentDetach(Entity entity)
	{
		const HierarchyComponent* parent = m_hierarchies.GetComponent(entity);

		if (parent != nullptr)
		{
			TransformComponent* transform = m_transforms.GetComponent(entity);
			if (transform != nullptr)
			{
				transform->ApplyTransform();
			}
			m_hierarchies.RemoveKeepSorted(entity);
		}
	}

	uint32_t Scene::GetComponentsMask(long entity)
	{
		uint32_t mask = 0;
		if (m_names.Contains(entity)) mask |= 1;
		if (m_transforms.Contains(entity)) mask |= 2;
		if (m_hierarchies.Contains(entity)) mask |= 4;
		if (m_meshes.Contains(entity)) mask |= 8;
		if (m_boundingBoxes.Contains(entity)) mask |= 16;
		if (m_materials.Contains(entity)) mask |= 32;

		return mask;
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

		for (size_t i = 0; i < m_hierarchies.GetCount(); ++i)
		{
			const HierarchyComponent& parentcomponent = m_hierarchies[i];
			Entity entity = m_hierarchies.GetEntity(i);

			TransformComponent* transformChild = m_transforms.GetComponent(entity);
			TransformComponent* transformParent = m_transforms.GetComponent(parentcomponent.m_parentId);
			if (transformChild != nullptr && transformParent != nullptr)
			{
				transformChild->UpdateTransformParented(*transformParent);
			}
		}
	}
}