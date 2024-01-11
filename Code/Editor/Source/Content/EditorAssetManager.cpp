#include "EditorAssetManager.h"
#include <filesystem>

namespace Bruno
{
	AssetMetadata EditorAssetManager::g_nullMetadata;

	EditorAssetManager::EditorAssetManager(const std::wstring& projectPath) :
		m_projectPath(projectPath)
	{
		m_importerManager.Initialize();
		ProcessDirectory(projectPath);
		GetAssetsDirectory(projectPath);
	}

	std::shared_ptr<Asset> EditorAssetManager::GetAsset(AssetHandle assetHandle)
	{
		auto& metadata = GetMetadata(assetHandle);
		if (!metadata) {
			return nullptr;
		}

		std::shared_ptr<Asset> asset = nullptr;
		if (!metadata.IsLoaded)
		{
			AssetImporterContext context(*this);
			if (!m_importerManager.TryImport(metadata, context, asset))
			{
				return nullptr;
			}
			metadata.IsLoaded = true;
			m_loadedAssets[assetHandle] = asset;
		}
		else
		{
			asset = m_loadedAssets[assetHandle];
		}
		return asset;
	}

	void EditorAssetManager::GetAssetsDirectory(const std::wstring& directoryPath)
	{
		for (auto& [handle, metadata] : m_assetTable) {
			auto asset = GetAsset(handle);
		}
	}

	void EditorAssetManager::ProcessDirectory(const std::wstring& directoryPath)
	{
		for (auto entry : std::filesystem::directory_iterator(directoryPath))
		{
			if (entry.is_directory())
			{
				ProcessDirectory(entry.path());
			}
			else
			{
				ImportAsset(entry.path());
			}
		}
	}

	const AssetMetadata& EditorAssetManager::GetMetadata(const std::wstring& filename)
	{
		std::filesystem::path relativePath = std::filesystem::relative(filename, m_projectPath);

		for (auto& [handle, metadata] : m_assetTable) {
			if (metadata.Filename == filename) {
				return metadata;
			}
		}
		return g_nullMetadata;
	}

	AssetMetadata& EditorAssetManager::GetMetadata(AssetHandle handle)
	{
		if (m_assetTable.Contains(handle))
			return m_assetTable[handle];

		return g_nullMetadata;
	}

	AssetHandle EditorAssetManager::ImportAsset(const std::wstring& filename)
	{
		std::filesystem::path relativePath = std::filesystem::relative(filename, m_projectPath);
		auto& metadata = GetMetadata(relativePath);
		if (metadata) {
			return metadata.Handle;
		}

		AssetType assetType = GetAssetTypeByExtension(relativePath.extension().string());
		if (assetType == AssetType::None)
			return 0;

		AssetMetadata newMetadata;
		newMetadata.Handle = {};
		newMetadata.Filename = relativePath;
		newMetadata.Type = assetType;
		m_assetTable[newMetadata.Handle] = newMetadata;

		return newMetadata.Handle;
	}

	AssetType EditorAssetManager::GetAssetTypeByExtension(const std::string& fileExtension)
	{
		auto it = g_AssetExtensionMap.find(fileExtension);
		if (it != g_AssetExtensionMap.end())
		{
			return it->second;
		}

		return AssetType::None;
	}
}