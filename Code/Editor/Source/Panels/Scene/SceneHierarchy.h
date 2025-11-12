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

		using Iterator = std::unordered_map<UUID, PropertyCollection>::iterator;
		using ConstIterator = std::unordered_map<UUID, PropertyCollection>::const_iterator;

		void LoadProperties(Entity rootEntity);

		Iterator begin() const { m_uuidToProperties.begin(); }
		Iterator end() const { m_uuidToProperties.end(); }
		PropertyCollection& operator[](const UUID& uuid) { return m_uuidToProperties[uuid]; }
		PropertyCollection& get(const UUID& uuid) { return m_uuidToProperties[uuid]; }
	private:

		std::unordered_map<UUID, PropertyCollection> m_uuidToProperties;
		std::shared_ptr<Scene> m_scene;
	};
}