#pragma once

#include "Bruno/Content/AssetMetadata.h"
#include "Bruno/Content/AssetTypes.h"
#include "Bruno/Content/Asset.h"
#include "AssetImporter.h"
#include "AssetImporterContext.h"

#include <unordered_map>
#include <memory>

namespace Bruno
{
	class ImporterManager
	{
	public:
		ImporterManager() = default;

		void Initialize();
		bool TryImport(const AssetMetadata& metadata, AssetImporterContext& context, std::shared_ptr<Asset>& asset);

	private:
		bool m_initialized{ false };
		std::unordered_map<AssetType, std::unique_ptr<AbstractAssetImporter>> m_serializers;
	};
}
