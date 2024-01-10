#pragma once

#include "AssetMetadata.h"
#include "AssetTypes.h"
#include "Asset.h"
#include "AssetImporter.h"

#include <unordered_map>
#include <memory>

namespace Bruno
{
	class ImporterManager
	{
	public:
		ImporterManager() = default;

		void Initialize();
		bool TryImport(const AssetMetadata& metadata, std::shared_ptr<Asset>& asset);

	private:
		bool m_initialized{ false };
		std::unordered_map<AssetType, std::unique_ptr<AbstractAssetImporter>> m_serializers;
	};
}
