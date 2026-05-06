#include "brepch.h"
#include "SceneHierarchy.h"

#include <Bruno/Scene/Entity.h>
#include <Bruno/Scene/Scene.h>

namespace Bruno
{
	SceneHierarchy::SceneHierarchy(std::shared_ptr<Scene> scene) :
		m_scene(scene)
	{
	}
/*
	void SceneHierarchy::LoadProperties(Entity entity)
	{
		PropertyCollection properties;

		auto& hierarchy = entity.GetComponent<HierarchyComponent>();
		
		auto& name = entity.GetComponent<NameComponent>().Name;
		properties.append("Name").SetLabel("Name").SetCategory("").SetType(PropertyGridType::String).SetValue(name);
		
		auto& transform = entity.GetComponent<TransformComponent>();
		properties.append("Transform/Position").SetLabel("Position").SetCategory("Transform").SetType(PropertyGridType::Vector3).SetValue(transform.Position);

		properties.append("Transform/Rotation").SetLabel("Rotation").SetCategory("Transform").SetType(PropertyGridType::Vector3).SetValue(transform.Rotation.ToEuler());

		properties.append("Transform/Scale").SetLabel("Scale").SetCategory("Transform").SetType(PropertyGridType::Vector3).SetValue(transform.Scale);

		if (entity.HasComponent<ModelComponent>())
		{
			auto& model = entity.GetComponent<ModelComponent>();
			properties.append("Model/Handle").SetLabel("Handle").SetCategory("Model").SetType(PropertyGridType::String).SetReadOnly(true).SetValue(model.ModelHandle);

			properties.append("Model/MeshIndex").SetLabel("Mesh index").SetCategory("Model").SetValue(model.MeshIndex).SetType(PropertyGridType::Uint).SetReadOnly(true);

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
				properties.append(propName).SetLabel(propLabel).SetCategory("Model").SetType(PropertyGridType::AssetFile).SetValue(materialAsset);
			}
		}
		m_uuidToProperties[entity.GetUUID()] = properties;

		for (UUID child : hierarchy.Children)
		{
			if (auto childEntity = m_scene->TryGetEntityWithUUID(child))
			{
				LoadProperties(childEntity);
			}
		}
	}
	*/
}