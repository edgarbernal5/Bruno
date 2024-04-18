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
		BR_RTTI_DECLARATION(EditorAssetManager, AbstractAssetManager);

	public:
		EditorAssetManager(const std::wstring& projectPath);

		void AddMemoryOnlyAsset(std::shared_ptr<Asset> asset) override;
		std::shared_ptr<Asset> GetAsset(AssetHandle assetHandle) override;
		std::wstring GetAbsolutePath(const std::wstring& path);

		AssetMetadata& GetMetadata(AssetHandle handle);
		const AssetMetadata& GetMetadata(const std::wstring& filename);
		AssetHandle ImportAsset(const std::wstring& filename);

	private:
		void GetAssetsDirectory(const std::wstring& directoryPath);
		void ProcessDirectory(const std::wstring& directoryPath);
		AssetType GetAssetTypeByExtension(const std::string& fileExtension);
		bool IsMemoryAsset(AssetHandle handle);

		std::wstring m_projectPath;
		std::unordered_map<AssetHandle, std::shared_ptr<Asset>> m_loadedAssets;
		std::unordered_map<AssetHandle, std::shared_ptr<Asset>> m_memoryAssets;
		AssetTable m_assetTable;
		ImporterManager m_importerManager;

		static AssetMetadata g_nullMetadata;
	};
}

