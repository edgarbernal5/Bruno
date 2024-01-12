#pragma once

#include <string>

namespace Bruno
{
	class EditorAssetManager;

	class AssetImporterContext
	{
	public:
		AssetImporterContext(EditorAssetManager& assetManager);

		std::wstring GetAbsolutePath(const std::wstring& path);
		AssetHandle ImportAsset(const std::wstring& filename);

	private:
		EditorAssetManager& m_assetManager;
	};
}