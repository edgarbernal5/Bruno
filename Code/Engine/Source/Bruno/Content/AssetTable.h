#pragma once

#include "AssetMetadata.h"
#include <unordered_map>

namespace Bruno
{
	class AssetTable
	{
	public:
		auto begin() { return m_assetTable.begin(); }
		auto end() { return m_assetTable.end(); }
		auto begin() const { return m_assetTable.cbegin(); }
		auto end() const { return m_assetTable.cend(); }

		AssetMetadata& operator[](const AssetHandle handle);
		AssetMetadata& Get(const AssetHandle handle);
		const AssetMetadata& Get(const AssetHandle handle) const;

	private:
		std::unordered_map<AssetHandle, AssetMetadata> m_assetTable;
	};
}

