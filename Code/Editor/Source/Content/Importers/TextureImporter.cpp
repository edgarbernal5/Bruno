#include "brepch.h"
#include "TextureImporter.h"

#include <Bruno/Platform/DirectX/Texture.h>

namespace Bruno
{
	bool TextureImporter::TryImport(const AssetMetadata& metadata, AssetImporterContext& context, std::shared_ptr<Asset>& asset)
	{
		auto assetFilename = context.GetAbsolutePath(metadata.Filename);
		asset = std::make_shared<Texture>(assetFilename);
		asset->SetHandle(metadata.Handle);

		return true;
	}
}