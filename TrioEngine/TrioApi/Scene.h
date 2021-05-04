#pragma once

#include "TrioApiRequisites.h"

#include "Object.h"
#include "Ecs/ComponentManager.h"
#include "Ecs/Components.h"
#include "Renderer/Camera.h"

#include <vector>
#include <string>
#include <map>

namespace TrioEngine
{
	class Model;

	class TRIO_API_EXPORT Scene
	{
	public:
		Scene();
		virtual ~Scene();

		void LoadFromModel(Model* model);
		
		inline ComponentManager<NameComponent>& GetNames() {
			return m_names;
		}
		inline ComponentManager<MeshComponent>& GetMeshes() {
			return m_meshes;
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

		Entity CreateMaterialEntity(std::string name);
		Entity CreateMesh(std::string name);

		ComponentManager<NameComponent> m_names;
		ComponentManager<TransformComponent> m_transforms;
		ComponentManager<HierarchyComponent> m_hierarchy;
		ComponentManager<MeshComponent> m_meshes;
		ComponentManager<MaterialComponent> m_materials;
	};

}
