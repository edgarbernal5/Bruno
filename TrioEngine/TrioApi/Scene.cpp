#include "stdafx.h"
#include "Scene.h"

#include "GameObject.h"

namespace TrioEngine
{
	Scene* Scene::m_instance = nullptr;

	Scene* Scene::Instance()
	{
		return m_instance;
	}

	Scene::Scene()
	{
		m_instance = this;

		//GameObject::Create("App")->AddComponent<App>();
	}

	Scene::~Scene()
	{
		m_added_objects.clear();
		m_removed_objects.clear();
		m_objects.clear();

		m_instance = nullptr;
	}

	void Scene::AddGameObject(const std::shared_ptr<GameObject>& obj)
	{
		m_added_objects.push_back(obj);
	}

	void Scene::RemoveGameObject(const std::shared_ptr<GameObject>& obj)
	{
		bool found = false;
		for (int i = 0; i < m_added_objects.size(); i++)
		{
			if (m_added_objects[i] == obj)
			{
				found = true;
				m_added_objects.erase(m_added_objects.begin() + i);
				break;
			}
		}

		if (!found)
		{
			m_removed_objects.push_back(obj);
		}
	}

	void Scene::Update()
	{
		for (auto& i : m_objects)
		{
			auto& obj = i.second;
			if (obj->IsActiveInTree())
			{
				obj->Update();
			}
		}

		do
		{
			std::vector<std::shared_ptr<GameObject>> added = m_added_objects;
			m_added_objects.clear();

			for (int i = 0; i < added.size(); ++i)
			{
				auto& obj = added[i];
				if (obj->IsActiveInTree())
				{
					obj->Update();
				}

				m_objects[obj->GetId()] = obj;
			}

			added.clear();

		} while (m_added_objects.size() > 0);

		for (int i = 0; i < m_removed_objects.size(); ++i)
		{
			const auto& obj = m_removed_objects[i];
			m_objects.erase(obj->GetId());
		}
		m_removed_objects.clear();

		//for (auto& i : m_objects)
		//{
		//	auto& obj = i.second;
		//	if (obj->IsActiveInTree())
		//	{
		//		obj->LateUpdate();
		//	}
		//}
	}

	std::shared_ptr<GameObject> Scene::GetGameObject(const GameObject* obj)
	{
		for (int i = 0; i < m_added_objects.size(); ++i)
		{
			const auto& obj_ref = m_added_objects[i];
			if (obj_ref.get() == obj)
			{
				return obj_ref;
			}
		}

		auto it = m_objects.find(obj->GetId());
		if (it != m_objects.end())
		{
			return it->second;
		}

		return std::shared_ptr<GameObject>();
	}
}