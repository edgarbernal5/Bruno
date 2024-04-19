#include "brepch.h"
#include "AssetImporterContext.h"

#include "EditorAssetManager.h"

namespace Bruno
{
	AssetImporterContext::AssetImporterContext(EditorAssetManager& assetManager) :
		m_assetManager(assetManager)
	{
	}

	void AssetImporterContext::AddMemoryOnlyAsset(std::shared_ptr<Asset> asset)
	{
		m_assetManager.AddMemoryOnlyAsset(asset);
	}

	std::wstring AssetImporterContext::GetAbsolutePath(const std::wstring& path)
	{
		return m_assetManager.GetAbsolutePath(path);
	}

	AssetHandle AssetImporterContext::ImportAsset(const std::wstring& filename)
	{
		return m_assetManager.ImportAsset(filename);
	}
}