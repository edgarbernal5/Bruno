#pragma once

#include "Bruno/Content/Asset.h"
#include "Bruno/Core/Assert.h"
#include <map>

namespace Bruno
{
	class MaterialMap
	{
	public:
		MaterialMap() = default;
		~MaterialMap() = default;

		void Clear();
		void ClearMaterial(uint32_t index);

		AssetHandle GetMaterial(uint32_t materialIndex) const
		{
			BR_ASSERT(HasMaterial(materialIndex));
			return m_materials.at(materialIndex);
		}

		bool HasMaterial(uint32_t materialIndex) const
		{
			return m_materials.find(materialIndex) != m_materials.end();
		}
		std::map<uint32_t, AssetHandle>& GetMaterials() { return m_materials; }
		const std::map<uint32_t, AssetHandle>& GetMaterials() const { return m_materials; }

		using Iterator = std::map<uint32_t, AssetHandle>::iterator;
		using ConstIterator = std::map<uint32_t, AssetHandle>::const_iterator;

		Iterator begin() const { m_materials.begin(); }
		Iterator end() const { m_materials.end(); }

		void SetMaterial(uint32_t index, AssetHandle material);
	private:

		std::map<uint32_t, AssetHandle> m_materials;
	};
}