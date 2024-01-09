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

			}
		}
	}

	const AssetMetadata& EditorAssetManager::GetMetadata(const std::wstring& filename)
	{
		std::filesystem::path relativePath = std::filesystem::relative(filename, m_projectPath);

		return g_nullMetadata;
	}

	AssetHandle EditorAssetManager::ImportAsset(const std::wstring& filename)
	{
		std::filesystem::path relativePath = std::filesystem::relative(filename, m_projectPath);
		auto& metadata = GetMetadata(relativePath);

		return AssetHandle();
	}
}