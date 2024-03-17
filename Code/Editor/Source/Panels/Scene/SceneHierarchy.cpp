#include "SceneHierarchy.h"

#include <Bruno/Scene/Entity.h>
#include <Bruno/Scene/Scene.h>

namespace Bruno
{
	SceneHierarchy::SceneHierarchy(std::shared_ptr<Scene> scene) :
		m_scene(scene)
	{
	}

	void SceneHierarchy::Load(Entity entity)
	{
		properties_collection properties;

		auto& hierarchy = entity.GetComponent<HierarchyComponent>();
		{
			auto& name = entity.GetComponent<NameComponent>().Name;
			auto prop = properties.append("Name");
			prop.value(name);
			prop.label("Name");
			prop.category("");
			prop.type(pg_type::string);
		}
		{
			auto& transform = entity.GetComponent<TransformComponent>();
			auto prop = properties.append("Transform/Position");
			prop.value(transform.Position);
			prop.label("Position");
			prop.category("Transform");
			prop.type(pg_type::vector3);
		}
		m_uuidToProperties[entity.GetUUID()] = properties;

		for (UUID child : hierarchy.Children)
		{
			auto childEntity = m_scene->TryGetEntityWithUUID(child);
			if (childEntity)
			{
				Load(childEntity);
			}
		}
	}
}