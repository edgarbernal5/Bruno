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

		void OnWindowSizeChanged(int width, int height);
		void Update();

		static Scene* GetActiveScene();
		static void SetActiveScene(Scene* scene);

		static Camera* GetCamera();
		static void UpdateCamera(const Camera& camera);

	private:
		static Scene* g_activeScene;
		static Camera* g_mainCamera;

		Entity CreateEntityForMaterial(std::string name);
		Entity CreateEntityForMesh(std::string name);
		Entity CreateEntityForBone(std::string name);

		void ComponentAttach(Entity entity, Entity parent, bool child_already_in_local_space);
		void ComponentDetach(Entity entity);

		ComponentManager<NameComponent> m_names;
		ComponentManager<TransformComponent> m_transforms;
		ComponentManager<HierarchyComponent> m_hierarchies;
		ComponentManager<MeshComponent> m_meshes;
		ComponentManager<MaterialComponent> m_materials;
	};

}
