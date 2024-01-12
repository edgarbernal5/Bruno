#include "brpch.h"
#include "AssetImporterContext.h"

#include "EditorAssetManager.h"

namespace Bruno
{
	AssetImporterContext::AssetImporterContext(EditorAssetManager& assetManager) :
		m_assetManager(assetManager)
	{
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