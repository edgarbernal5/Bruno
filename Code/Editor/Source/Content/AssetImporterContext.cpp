#include "brepch.h"
#include "AssetImporterContext.h"

#include <utility>

#include "EditorAssetManager.h"

namespace Bruno
{
	AssetImporterContext::AssetImporterContext(EditorAssetManager& assetManager) :
		m_assetManager(assetManager)
	{
	}

	void AssetImporterContext::AddMemoryOnlyAsset(std::shared_ptr<Asset> asset) const
	{
		m_assetManager.AddMemoryOnlyAsset(std::move(asset));
	}

	std::wstring AssetImporterContext::GetAbsolutePath(const std::wstring& path) const
	{
		return m_assetManager.GetAbsolutePath(path);
	}

	AssetHandle AssetImporterContext::ImportAsset(const std::wstring& filename) const
	{
		return m_assetManager.ImportAsset(filename);
	}
}