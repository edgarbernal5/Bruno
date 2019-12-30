#pragma once

#include "TrioApiRequisites.h"
#include "Object.h"
#include <vector>
#include <string>
#include <map>

namespace TrioEngine
{
	class GameObject;

	class Scene : public Object
	{
	public:
		Scene();
		virtual ~Scene();
		
		std::shared_ptr<GameObject> GetGameObject(const GameObject* object);
		void Update();

		static Scene* ActiveScene();

		friend class GameObject;
	private:
		static Scene* g_activeScene;

		void AddGameObject(const std::shared_ptr<GameObject>& object);
		void RemoveGameObject(const std::shared_ptr<GameObject>& object);

		std::vector<Scene*> m_scenes;
		std::map<int, std::shared_ptr<GameObject>> m_objects;
		std::vector<std::shared_ptr<GameObject>> m_addedObjects;
		std::vector<std::shared_ptr<GameObject>> m_removedObjects;
	};
}
