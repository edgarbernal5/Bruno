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
		
		auto& name = entity.GetComponent<NameComponent>().Name;
		properties.append("Name").label("Name").category("").type(pg_type::string).value(name);
		
		auto& transform = entity.GetComponent<TransformComponent>();
		properties.append("Transform/Position").label("Position").category("Transform").type(pg_type::vector3).value(transform.Position);

		properties.append("Transform/Rotation").label("Rotation").category("Transform").type(pg_type::vector3).value(transform.Rotation.ToEuler());

		properties.append("Transform/Scale").label("Scale").category("Transform").type(pg_type::vector3).value(transform.Scale);

		if (entity.HasComponent<ModelComponent>())
		{
			auto& model = entity.GetComponent<ModelComponent>();
			properties.append("Model/Handle").label("Handle").category("Model").type(pg_type::string).read_only(true).value(model.ModelHandle);

			properties.append("Model/MeshIndex").label("Mesh index").category("Model").value(model.MeshIndex).type(pg_type::uint).read_only(true);

			for (auto& [index, materialAsset] : model.Materials->GetMaterials())
			{
				std::string propName;
				{
					std::ostringstream oss;
					oss << "Model/Materials/" << index;
					propName = oss.str();
				}
				std::string propLabel;
				{
					std::ostringstream oss;
					oss << "Material " << index;
					propLabel = oss.str();
				}
				properties.append(propName).label(propLabel).category("Model").type(pg_type::asset_file).value(materialAsset);
			}
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