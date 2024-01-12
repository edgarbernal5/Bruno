#include "brpch.h"
#include "TextureImporter.h"

#include "Bruno/Core/StringHelpers.h"

namespace Bruno
{
	TextureImporter::TextureImporter()
	{
		
	}

	bool TextureImporter::TryImport(const AssetMetadata& metadata, AssetImporterContext& context, std::shared_ptr<Asset>& asset)
	{
		return false;
	}
}