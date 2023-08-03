#pragma once

#include <map>
#include <Bruno/Core/RTTI.h>

namespace Bruno
{
	class ContentManager
	{
	public:
		ContentManager(const std::wstring& rootDirectory = DefaultRootDirectory);

		template <typename T>
		std::shared_ptr<T> Load(const std::wstring& assetName, bool forceReload = false);
	private:
		std::wstring m_rootDirectory;
		std::map<std::wstring, std::shared_ptr<RTTI>> m_loadedAssets;

		std::shared_ptr<RTTI> ReadAsset(const RTTI::IdType targetTypeId, const std::wstring& assetName);

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

		auto targetTypeId = T::GetTypeIdClass();
		std::filesystem::path assetPath = m_rootDirectory;
		assetPath /= assetName;

		auto asset = ReadAsset(targetTypeId, assetPath);
		m_loadedAssets[assetName] = asset;

		return std::static_pointer_cast<T>(asset);
	}
}

