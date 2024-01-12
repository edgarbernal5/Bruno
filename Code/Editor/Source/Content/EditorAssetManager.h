#pragma once

#include <string>
#include "Bruno/Content/AssetMetadata.h"
#include "Bruno/Content/AssetManager.h"
#include "Bruno/Content/AssetTable.h"
#include "Bruno/Content/AssetExtensions.h"
#include "ImporterManager.h"

namespace Bruno
{
	class EditorAssetManager : public AbstractAssetManager
	{
	public:
		EditorAssetManager(const std::wstring& projectPath);

		std::shared_ptr<Asset> GetAsset(AssetHandle assetHandle) override;
		std::wstring GetAbsolutePath(const std::wstring& path);

		AssetHandle ImportAsset(const std::wstring& filename);

	private:
		void GetAssetsDirectory(const std::wstring& directoryPath);
		void ProcessDirectory(const std::wstring& directoryPath);
		const AssetMetadata& GetMetadata(const std::wstring& filename);
		AssetMetadata& GetMetadata(AssetHandle handle);
		AssetType GetAssetTypeByExtension(const std::string& fileExtension);

		std::wstring m_projectPath;
		std::unordered_map<AssetHandle, std::shared_ptr<Asset>> m_loadedAssets;
		AssetTable m_assetTable;
		ImporterManager m_importerManager;

		static AssetMetadata g_nullMetadata;
	};
}

