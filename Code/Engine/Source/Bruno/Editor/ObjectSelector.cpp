#include "brpch.h"
#include "ObjectSelector.h"

#include "Bruno/Scene/Scene.h"

namespace Bruno
{
	ObjectSelector::ObjectSelector(std::shared_ptr<Scene> scene) : m_scene(scene)
	{

	}

	std::vector<std::shared_ptr<Transformable>>& ObjectSelector::GetSelectedObjects() {
		return m_scene->m_transformableObjects;
	}
}