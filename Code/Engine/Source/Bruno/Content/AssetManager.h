#pragma once

#include "Asset.h"
#include "Bruno/Core/RTTI.h"

namespace Bruno
{
	class AbstractAssetManager : public RTTI
	{
		BR_RTTI_DECLARATION(AbstractAssetManager, RTTI);

	public:
		~AbstractAssetManager() override = default;

		virtual void AddMemoryOnlyAsset(std::shared_ptr<Asset> asset) = 0;
		//virtual bool IsMemoryAsset(AssetHandle handle) = 0;
		
		virtual std::shared_ptr<Asset> GetAssetRaw(AssetHandle assetHandle) = 0;
		
		template<typename TAsset>
		AssetHandle AddMemoryOnlyAsset(std::shared_ptr<TAsset> asset)
		{
			static_assert(std::is_base_of<Asset, TAsset>::value, "AddMemoryOnlyAsset only works for types derived from Asset");
			asset->Handle = {};

			AddMemoryOnlyAsset(asset);
			return asset->Handle;
		}

		template<typename TAsset, typename... TArgs>
		AssetHandle AddMemoryOnlyAsset(TArgs&&... args)
		{
			static_assert(std::is_base_of<Asset, TAsset>::value, "AddMemoryOnlyAsset only works for types derived from Asset");
			
			auto asset = std::make_shared<TAsset>(std::forward<TArgs>(args)...);
			asset->Handle = {};

			AddMemoryOnlyAsset(asset);
			return asset->Handle;
		}

		template<typename T>
		std::shared_ptr<T> GetAsset(AssetHandle assetHandle)
		{
			// Bonus C++17: Asegurarnos de que T herede de Asset
			static_assert(std::is_base_of_v<Asset, T>, "GetAsset only works for types derived from Asset");
			
			return std::static_pointer_cast<T>(GetAssetRaw(assetHandle));
		}

	protected:
		
	};
}
