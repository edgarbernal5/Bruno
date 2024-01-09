#pragma once

#include <string>
#include "Bruno/Content/AssetMetadata.h"
#include "Bruno/Content/AssetManager.h"
#include "Bruno/Content/AssetTable.h"

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

		std::wstring m_projectPath;
		AssetTable m_assetTable;

		static AssetMetadata g_nullMetadata;
	};
}

