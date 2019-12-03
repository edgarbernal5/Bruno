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
		static Scene* Instance();
		Scene();
		virtual ~Scene();
		
		void Update();
		std::shared_ptr<GameObject> GetGameObject(const GameObject* obj);

	private:
		friend class GameObject;
		void AddGameObject(const std::shared_ptr<GameObject>& obj);
		void RemoveGameObject(const std::shared_ptr<GameObject>& obj);

	private:
		static Scene* m_instance;

		std::map<int, std::shared_ptr<GameObject>> m_objects;
		std::vector<std::shared_ptr<GameObject>> m_added_objects;
		std::vector<std::shared_ptr<GameObject>> m_removed_objects;
	};
}
