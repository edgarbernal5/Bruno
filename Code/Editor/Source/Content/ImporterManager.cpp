#include "brpch.h"
#include "ImporterManager.h"

#include "AssetImporter.h"
#include "Importers/ModelImporter.h"

namespace Bruno
{
	void ImporterManager::Initialize()
	{
		if (m_initialized)
			return;

		m_serializers[AssetType::Model] = std::make_unique<ModelImporter>();
		m_initialized = true;
	}

	bool ImporterManager::TryImport(const AssetMetadata& metadata, AssetImporterContext& context, std::shared_ptr<Asset>& asset)
	{
		auto it = m_serializers.find(metadata.Type);
		if (it == m_serializers.end())
		{
			BR_CORE_WARN << "There is no importer for assets of type: " << GetAssetTypeStringFrom(metadata.Type) << std::endl;
			return false;
		}

		return it->second->TryImport(metadata, context, asset);
	}
}