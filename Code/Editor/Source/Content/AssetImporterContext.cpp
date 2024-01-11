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
		std::filesystem::path absolutePath = m_assetManager.GetProjectPath();
		absolutePath /= path;
		return absolutePath;
	}
}