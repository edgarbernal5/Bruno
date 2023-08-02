#pragma once

#include <map>
#include <Bruno/Core/RTTI.h>

namespace Bruno
{
	class ContentManager
	{
	public:
		ContentManager(const std::string& rootDirectory = DefaultRootDirectory);

		template <typename T>
		std::shared_ptr<T> Load(const std::string& assetName, bool forceReload = false);
	private:
		std::string m_rootDirectory;
		std::map<std::string, std::shared_ptr<RTTI>> m_loadedAssets;

		std::shared_ptr<RTTI> ReadAsset(const RTTI::IdType targetTypeId, const std::string& assetName);

		static const std::string DefaultRootDirectory;
	};

	template<typename T>
	inline std::shared_ptr<T> ContentManager::Load(const std::string& assetName, bool forceReload)
	{
		if (!forceReload)
		{
			auto it = m_loadedAssets.find(assetName);
			if (it != m_loadedAssets.end())
			{
				return std::static_pointer_cast<T>(it->second);
			}
		}

		auto targetTypeId = T::GetTypeIdClass();
		auto pathName = m_rootDirectory + assetName;

		auto asset = ReadAsset(targetTypeId, pathName);
		m_loadedAssets[assetName] = asset;

		return std::static_pointer_cast<T>(asset);
	}
}

