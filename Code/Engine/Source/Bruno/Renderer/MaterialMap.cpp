#include "brpch.h"
#include "MaterialMap.h"

namespace Bruno
{
	void MaterialMap::Clear()
	{
		m_materials.clear();
	}

	void MaterialMap::ClearMaterial(uint32_t index)
	{
		BR_ASSERT(index < m_materials.size());
		m_materials.erase(index);
	}

	void MaterialMap::SetMaterial(uint32_t index, AssetHandle material)
	{
		m_materials[index] = material;
	}
}