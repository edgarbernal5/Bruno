#include "brpch.h"
#include "AssetTable.h"

namespace Bruno
{
	AssetMetadata& AssetTable::operator[](const AssetHandle handle)
	{
		return m_assetTable[handle];
	}

	AssetMetadata& AssetTable::Get(const AssetHandle handle)
	{
		return m_assetTable[handle];
	}

	const AssetMetadata& AssetTable::Get(const AssetHandle handle) const
	{
		return m_assetTable.at(handle);
	}

	bool AssetTable::Contains(const AssetHandle handle) const
	{
		return m_assetTable.find(handle) != m_assetTable.end();
	}
}