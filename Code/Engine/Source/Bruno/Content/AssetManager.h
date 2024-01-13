#pragma once

#include "Asset.h"
#include "Bruno/Core/RTTI.h"

namespace Bruno
{
	class AbstractAssetManager : public RTTI
	{
		BR_RTTI_DECLARATION(AbstractAssetManager, RTTI);

	public:
		virtual ~AbstractAssetManager() = default;

		virtual std::shared_ptr<Asset> GetAsset(AssetHandle assetHandle) = 0;
	protected:
		
	};
}
