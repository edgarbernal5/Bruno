#pragma once

#include "TrioApiRequisites.h"

#include "Object.h"
#include "Ecs/ComponentManager.h"
#include "Ecs/Components.h"
#include "Renderer/Camera.h"

#include <vector>
#include <string>
#include <map>

namespace BrunoEngine
{
	class Model;

	class BRUNO_API_EXPORT Scene
	{
	public:
		Scene();
		virtual ~Scene();

		void LoadFromModel(Model* model);

		inline ComponentManager<MaterialComponent>& GetMaterials() {
			return m_materials;
		}
		inline ComponentManager<MeshComponent>& GetMeshes() {
			return m_meshes;
		}
		inline ComponentManager<NameComponent>& GetNames() {
			return m_names;
		}
		inline ComponentManager<TransformComponent>& GetTransforms() {
			return m_transforms;
		}
		inline ComponentManager<HierarchyComponent>& GetHierarchies() {
			return m_hierarchies;
		}
		inline ComponentManager<BoundingBoxComponent>& GetBoundingBoxes() {
			return m_boundingBoxes;
		}
		inline ComponentManager<LightComponent>& GetLights() {
			return m_lights;
		}

		Entity AddEmptyObject(std::string name);
		uint32_t GetComponentsMask(Entity entity);

		void OnWindowSizeChanged(int width, int height);
		void Update();

		void RemoveEntity(Entity entity);

		static Scene* GetActiveScene();
		static void SetActiveScene(Scene* scene);

		static Camera* GetCamera();
		static void UpdateCamera(const Camera& camera);

		void UpdateTransformFor(Entity entity);
	private:
		static Scene* g_activeScene;
		static Camera* g_mainCamera;

		Entity CreateEntityForMaterial(std::string name);
		Entity CreateEntityForMesh(std::string name);
		Entity CreateEntityForEmptyObject(std::string name);

		void ComponentAttach(Entity entity, Entity parent, bool childAlreadyInLocalSpace);
		void ComponentDetach(Entity entity);
		void RemoveEntity(Entity child, Entity parent, std::vector<Entity>& toErase);

		ComponentManager<NameComponent> m_names;
		ComponentManager<TransformComponent> m_transforms;
		ComponentManager<HierarchyComponent> m_hierarchies;
		ComponentManager<MeshComponent> m_meshes;
		ComponentManager<BoundingBoxComponent> m_boundingBoxes;
		ComponentManager<MaterialComponent> m_materials;
		ComponentManager<LightComponent> m_lights;
	};
}
