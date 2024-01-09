#pragma once

#include <string>
#include "Bruno/Content/AssetMetadata.h"
#include "Bruno/Content/AssetManager.h"
#include "Bruno/Content/AssetTable.h"
#include "Bruno/Content/AssetExtensions.h"

namespace Bruno
{
	class EditorAssetManager : public AbstractAssetManager
	{
	public:
		EditorAssetManager(const std::wstring& projectPath);

		std::shared_ptr<Asset> GetAsset(AssetHandle assetHandle) override;

	private:
		void ProcessDirectory(const std::wstring& directoryPath);
		const AssetMetadata& GetMetadata(const std::wstring& filename);
		AssetHandle ImportAsset(const std::wstring& filename);
		AssetType GetAssetTypeByExtension(const std::string& fileExtension);

		std::wstring m_projectPath;
		AssetTable m_assetTable;

		static AssetMetadata g_nullMetadata;
	};
}

