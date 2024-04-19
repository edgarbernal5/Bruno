#pragma once

#include <string>
#include <Bruno/Content/Asset.h>

namespace Bruno
{
	class EditorAssetManager;

	class AssetImporterContext
	{
	public:
		AssetImporterContext(EditorAssetManager& assetManager);

		void AddMemoryOnlyAsset(std::shared_ptr<Asset> asset);
		std::wstring GetAbsolutePath(const std::wstring& path);
		AssetHandle ImportAsset(const std::wstring& filename);
	private:
		EditorAssetManager& m_assetManager;
	};
}