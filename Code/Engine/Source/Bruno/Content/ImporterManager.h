#pragma once

#include "AssetMetadata.h"
#include "AssetTypes.h"
#include "Asset.h"
#include "AssetImportSerializer.h"

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
		std::unordered_map<AssetType, std::unique_ptr<AbstractAssetImportSerializer>> m_serializers;
	};
}
