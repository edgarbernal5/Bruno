#include "EditorAssetManager.h"
#include <filesystem>

namespace Bruno
{
	AssetMetadata EditorAssetManager::g_nullMetadata;

	EditorAssetManager::EditorAssetManager(const std::wstring& projectPath) :
		m_projectPath(projectPath)
	{
		ProcessDirectory(projectPath);
	}

	std::shared_ptr<Asset> EditorAssetManager::GetAsset(AssetHandle assetHandle)
	{

		return std::shared_ptr<Asset>();
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
		newMetadata.Filename = filename;
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