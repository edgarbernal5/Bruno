#include "SceneHierarchy.h"

#include <Bruno/Scene/Entity.h>
#include <Bruno/Scene/Scene.h>

namespace Bruno
{
	SceneHierarchy::SceneHierarchy(std::shared_ptr<Scene> scene) :
		m_scene(scene)
	{
	}

	void SceneHierarchy::LoadProperties(Entity entity)
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
			auto propPosition = properties.append("Transform/Position");
			propPosition.value(transform.Position);
			propPosition.label("Position");
			propPosition.category("Transform");
			propPosition.type(pg_type::vector3);

			auto propRotation = properties.append("Transform/Rotation");
			propRotation.value(transform.Rotation.ToEuler());
			propRotation.label("Rotation");
			propRotation.category("Transform");
			propRotation.type(pg_type::vector3);

			auto propScale = properties.append("Transform/Scale");
			propScale.value(transform.Scale);
			propScale.label("Scale");
			propScale.category("Transform");
			propScale.type(pg_type::vector3);
		}

		if (entity.HasComponent<ModelComponent>())
		{
			auto& model = entity.GetComponent<ModelComponent>();
			auto propHandle = properties.append("Model/Handle");
			propHandle.value(model.ModelHandle).label("Handle").category("Model").type(pg_type::string).read_only(true);

			auto propMeshIndex = properties.append("Model/MeshIndex");
			propMeshIndex.value(model.MeshIndex).label("Mesh index").category("Model").type(pg_type::uint).read_only(true);
		}
		m_uuidToProperties[entity.GetUUID()] = properties;

		for (UUID child : hierarchy.Children)
		{
			auto childEntity = m_scene->TryGetEntityWithUUID(child);
			if (childEntity)
			{
				LoadProperties(childEntity);
			}
		}
	}
}