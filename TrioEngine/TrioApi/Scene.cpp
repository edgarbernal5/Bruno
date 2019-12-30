#include "stdafx.h"
#include "Scene.h"

#include "GameObject.h"

namespace TrioEngine
{
	Scene* Scene::g_activeScene = nullptr;
	
	Scene::Scene()
	{
		m_scenes.push_back(this);
		if (g_activeScene == nullptr)
		{
			g_activeScene = this;
		}
	}

	Scene::~Scene()
	{
		m_addedObjects.clear();
		m_removedObjects.clear();
		m_objects.clear();

		for (int i = 0; i < m_scenes.size(); ++i)
		{
			if (m_scenes[i] == this)
			{
				m_scenes.erase(m_scenes.begin() + i);
				break;
			}
		}
		if (g_activeScene == this)
		{
			if (m_scenes.size() == 0)
			{
				g_activeScene = nullptr;
			}
			else
			{
				g_activeScene = m_scenes[0];
			}
		}
	}

	Scene* Scene::ActiveScene()
	{
		return g_activeScene;
	}

	void Scene::AddGameObject(const std::shared_ptr<GameObject>& obj)
	{
		m_addedObjects.push_back(obj);
	}

	void Scene::RemoveGameObject(const std::shared_ptr<GameObject>& obj)
	{
		bool found = false;
		for (int i = 0; i < m_addedObjects.size(); i++)
		{
			if (m_addedObjects[i] == obj)
			{
				found = true;
				m_addedObjects.erase(m_addedObjects.begin() + i);
				break;
			}
		}

		if (!found)
		{
			m_removedObjects.push_back(obj);
		}
	}

	void Scene::Update()
	{
		for (auto& item : m_objects)
		{
			auto& object = item.second;
			if (object->IsActiveInTree())
			{
				object->Update();
			}
		}

		do
		{
			std::vector<std::shared_ptr<GameObject>> added = m_addedObjects;
			m_addedObjects.clear();

			for (int i = 0; i < added.size(); ++i)
			{
				auto& object = added[i];
				if (object->IsActiveInTree())
				{
					object->Update();
				}

				m_objects[object->GetId()] = object;
			}

			added.clear();

		} while (m_addedObjects.size() > 0);

		for (int i = 0; i < m_removedObjects.size(); ++i)
		{
			const auto& object = m_removedObjects[i];
			m_objects.erase(object->GetId());
		}
		m_removedObjects.clear();
	}

	std::shared_ptr<GameObject> Scene::GetGameObject(const GameObject* object)
	{
		for (int i = 0; i < m_addedObjects.size(); ++i)
		{
			const auto& objectRef = m_addedObjects[i];
			if (objectRef.get() == object)
			{
				return objectRef;
			}
		}

		auto it = m_objects.find(object->GetId());
		if (it != m_objects.end())
		{
			return it->second;
		}

		return std::shared_ptr<GameObject>();
	}
}