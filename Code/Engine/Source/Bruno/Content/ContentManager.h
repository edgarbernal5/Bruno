#pragma once

#include <map>
#include "Asset.h"

namespace Bruno
{
	//class Asset;

	class ContentManager
	{
	public:
		ContentManager(const std::wstring& rootDirectory = DefaultRootDirectory);

		template <typename T>
		std::shared_ptr<T> Load(const std::wstring& assetName, bool forceReload = false);
	private:
		std::shared_ptr<Asset> ReadAsset(const std::wstring& assetName);

		std::wstring m_rootDirectory;
		std::map<std::wstring, std::shared_ptr<Asset>> m_loadedAssets;

		static const std::wstring DefaultRootDirectory;
	};

	template<typename T>
	inline std::shared_ptr<T> ContentManager::Load(const std::wstring& assetName, bool forceReload)
	{
		if (!forceReload)
		{
			auto it = m_loadedAssets.find(assetName);
			if (it != m_loadedAssets.end())
			{
				return std::static_pointer_cast<T>(it->second);
			}
		}

		std::filesystem::path assetPath = m_rootDirectory;
		assetPath /= assetName;

		//TODO: AssetMetadata. Separar content manager en runtime y editor

		auto asset = ReadAsset(assetPath);
		m_loadedAssets[assetName] = asset;

		return std::static_pointer_cast<T>(asset);
	}
}

