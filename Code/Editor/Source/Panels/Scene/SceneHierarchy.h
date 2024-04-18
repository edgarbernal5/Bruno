#pragma once

#include <unordered_map>
#include <string>
#include <Bruno/Core/UUID.h>
#include "Panels/Properties/Property.h"

namespace Bruno
{
	class Entity;
	class Scene;

	//class HierarchyProxy : public std::iterator<std::input_iterator_tag, HierarchyProxy>
	//{
	//public:
	//	HierarchyProxy() = default;
	//	HierarchyProxy(const HierarchyProxy&) = default;
	//};

	class SceneHierarchy
	{
	public:
		SceneHierarchy(std::shared_ptr<Scene> scene);

		using Iterator = std::unordered_map<UUID, properties_collection>::iterator;
		using ConstIterator = std::unordered_map<UUID, properties_collection>::const_iterator;

		void LoadProperties(Entity rootEntity);

		Iterator begin() const { m_uuidToProperties.begin(); }
		Iterator end() const { m_uuidToProperties.end(); }
		properties_collection& operator[](const UUID& uuid) { return m_uuidToProperties[uuid]; }
		properties_collection& get(const UUID& uuid) { return m_uuidToProperties[uuid]; }
	private:

		std::unordered_map<UUID, properties_collection> m_uuidToProperties;
		std::shared_ptr<Scene> m_scene;
	};
}