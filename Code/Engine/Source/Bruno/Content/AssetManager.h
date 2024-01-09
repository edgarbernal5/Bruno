#pragma once

#include "Asset.h"

namespace Bruno
{
	class AbstractAssetManager
	{
	public:
		virtual ~AbstractAssetManager() = default;

		virtual std::shared_ptr<Asset> GetAsset(AssetHandle assetHandle) = 0;
	protected:
		
	};
}
