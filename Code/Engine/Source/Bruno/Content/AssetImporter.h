#pragma once

#include "AssetMetadata.h"

namespace Bruno
{
	class AbstractAssetImporter
	{
	public:
		virtual ~AbstractAssetImporter() = default;

		virtual bool TryImport(const AssetMetadata& metadata, std::shared_ptr<Asset>& asset) = 0;
	};
}

