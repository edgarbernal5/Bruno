#pragma once

#include <unordered_map>
#include <string>
#include <Bruno/Core/UUID.h>

namespace Bruno
{
	class Entity;
	class Scene;

	class SceneHierarchy
	{
	public:
		SceneHierarchy(std::shared_ptr<Scene> scene);

		

		//void LoadProperties(Entity rootEntity);

	private:

		std::shared_ptr<Scene> m_scene;
	};
}