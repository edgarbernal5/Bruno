#pragma once

#include "Bruno/Content/AssetMetadata.h"
#include "AssetImporterContext.h"

namespace Bruno
{
	class AbstractAssetImporter
	{
	public:
		virtual ~AbstractAssetImporter() = default;

		virtual bool TryImport(const AssetMetadata& metadata, AssetImporterContext& context, std::shared_ptr<Asset>& asset) = 0;
	};
}

