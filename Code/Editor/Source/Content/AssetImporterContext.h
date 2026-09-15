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

		void AddMemoryOnlyAsset(std::shared_ptr<Asset> asset) const;
		std::wstring GetAbsolutePath(const std::wstring& path) const;
		AssetHandle ImportAsset(const std::wstring& filename) const;
	private:
		EditorAssetManager& m_assetManager;
	};
}