#pragma once

#include "AssetMetadata.h"

namespace Bruno
{
	class AbstractAssetImportSerializer
	{
	public:
		virtual ~AbstractAssetImportSerializer() = default;

		virtual void Serialize(const std::shared_ptr<Asset>& asset) = 0;
		virtual bool TryDeserialize(const AssetMetadata& metadata, std::shared_ptr<Asset>& asset) = 0;
	};
}

