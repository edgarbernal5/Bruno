#pragma once

#include <vector>
#include "Bruno/Math/Math.h"

namespace Bruno
{
	struct Transformable;
	class Scene;

	class ObjectSelector
	{
	public:
		ObjectSelector(std::shared_ptr<Scene> scene);
		std::vector<std::shared_ptr<Transformable>>& GetSelectedObjects();

	private:
		std::shared_ptr<Scene> m_scene;
		//std::vector<Transformable*> m_objects;
	};
}

