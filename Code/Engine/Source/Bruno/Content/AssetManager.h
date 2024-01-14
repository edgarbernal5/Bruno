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
		
		template<typename T>
		std::shared_ptr<T> GetAsset(AssetHandle assetHandle);

	protected:
		
	};

	template<typename T>
	inline std::shared_ptr<T> AbstractAssetManager::GetAsset(AssetHandle assetHandle)
	{
		return std::static_pointer_cast<T>(GetAsset(assetHandle));
	}
}
