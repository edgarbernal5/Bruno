#pragma once

#include "TrioApiRequisites.h"

#include "Entity.h"

#include <vector>
#include <unordered_map>
#include <bitset>

namespace BrunoEngine
{
	/*
		1) https://www.david-colson.com/2020/02/09/making-a-simple-ecs.html
		2) https://github.com/SRombauts/ecs
	*/

	//template <class T>
	//int GetUniqueComponentId()
	//{
	//	static int g_componentId = 0;
	//	g_componentId++;
	//	return g_componentId;
	//}

	//const int MAX_COMPONENTS = 32;
	//const int MAX_ENTITIES = 1024;
	//typedef std::bitset<MAX_COMPONENTS> ComponentMask;

	template<typename Component>
	class ComponentManager
	{
	public:
		ComponentManager(size_t reservedCount = 0)
		{
			m_components.reserve(reservedCount);
			m_entities.reserve(reservedCount);
			m_lookup.reserve(reservedCount);
		}

		~ComponentManager()
		{
			Clear();
		}

		inline void Clear()
		{
			m_components.clear();
			m_entities.clear();
			m_lookup.clear();
		}

		inline Component& Create(Entity entity)
		{
			m_lookup[entity] = m_components.size();

			m_components.emplace_back();

			m_entities.push_back(entity);

			return m_components.back();
		}

		inline bool Contains(Entity entity) const
		{
			return m_lookup.find(entity) != m_lookup.end();
		}

		inline Component* GetComponent(Entity entity)
		{
			auto it = m_lookup.find(entity);
			if (it != m_lookup.end())
			{
				return &m_components[it->second];
			}
			return nullptr;
		}

		inline const Component* GetComponent(Entity entity) const
		{
			const auto it = m_lookup.find(entity);
			if (it != m_lookup.end())
			{
				return &m_components[it->second];
			}
			return nullptr;
		}

		inline size_t GetIndex(Entity entity) const
		{
			const auto it = m_lookup.find(entity);
			if (it != m_lookup.end())
			{
				return it->second;
			}
			return ~0;
		}

		inline size_t GetCount() const { return m_components.size(); }

		inline Entity GetEntity(size_t index) const { return m_entities[index]; }

		inline void MoveItem(size_t indexFrom, size_t indexTo)
		{
			if (indexFrom == indexTo)
			{
				return;
			}

			// Save the moved component and entity:
			Component component = std::move(m_components[indexFrom]);
			Entity entity = m_entities[indexFrom];

			// Every other entity-component that's in the way gets moved by one and lut is kept updated:
			const int direction = indexFrom < indexTo ? 1 : -1;
			for (size_t i = indexFrom; i != indexTo; i += direction)
			{
				const size_t next = i + direction;
				m_components[i] = std::move(m_components[next]);
				m_entities[i] = m_entities[next];
				m_lookup[m_entities[i]] = i;
			}

			// Saved entity-component moved to the required position:
			m_components[indexTo] = std::move(component);
			m_entities[indexTo] = entity;
			m_lookup[entity] = indexTo;
		}

		inline void Remove(Entity entity)
		{
			auto it = m_lookup.find(entity);
			if (it != m_lookup.end())
			{
				// Directly index into components and entities array:
				const size_t index = it->second;
				const Entity entity = m_entities[index];

				if (index < m_components.size() - 1)
				{
					// Swap out the dead element with the last one:
					m_components[index] = std::move(m_components.back()); // try to use move instead of copy
					m_entities[index] = m_entities.back();

					// Update the lookup table:
					m_lookup[m_entities[index]] = index;
				}

				// Shrink the container:
				m_components.pop_back();
				m_entities.pop_back();
				m_lookup.erase(entity);
			}
		}
		
		inline void RemoveKeepSorted(Entity entity)
		{
			auto it = m_lookup.find(entity);
			if (it != m_lookup.end())
			{
				// Directly index into components and entities array:
				const size_t index = it->second;
				const Entity entity = m_entities[index];

				if (index < m_components.size() - 1)
				{
					// Move every component left by one that is after this element:
					for (size_t i = index + 1; i < m_components.size(); ++i)
					{
						m_components[i - 1] = std::move(m_components[i]);
					}
					// Move every entity left by one that is after this element and update lut:
					for (size_t i = index + 1; i < m_entities.size(); ++i)
					{
						m_entities[i - 1] = m_entities[i];
						m_lookup[m_entities[i - 1]] = i - 1;
					}
				}

				// Shrink the container:
				m_components.pop_back();
				m_entities.pop_back();
				m_lookup.erase(entity);
			}
		}

		inline Component& operator[](size_t index) { return m_components[index]; }

		inline const Component& operator[](size_t index) const { return m_components[index]; }
	private:
		ComponentManager(const ComponentManager&) = delete;

		std::vector<Component> m_components;
		std::vector<Entity> m_entities;
		std::unordered_map<Entity, size_t> m_lookup;
	};
}