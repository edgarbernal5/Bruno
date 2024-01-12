#pragma once

#include "Content/AssetImporter.h"
#include "Content/AssetImporterContext.h"

namespace Bruno
{
	class TextureImporter : public AbstractAssetImporter
	{
	public:
		TextureImporter() = default;
		bool TryImport(const AssetMetadata& metadata, AssetImporterContext& context, std::shared_ptr<Asset>& asset) override;

	private:
		
	};
}

