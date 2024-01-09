#include "brpch.h"
#include "ImporterManager.h"

#include "AssetImportSerializer.h"

namespace Bruno
{
	void ImporterManager::Initialize()
	{
	}

	bool ImporterManager::TryImport(const AssetMetadata& metadata, std::shared_ptr<Asset>& asset)
	{
		auto it = m_serializers.find(metadata.Type);
		if (it == m_serializers.end())
		{
			BR_CORE_WARN << "There is no importer for assets of type: " << GetAssetTypeStringFrom(metadata.Type) << std::endl;
			return false;
		}

		return it->second->TryDeserialize(metadata, asset);
	}
}