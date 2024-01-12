#pragma once

#include "Content/AssetImporter.h"
#include "Bruno/Math/Math.h"

#include "Bruno/Renderer/Model.h"
#include "Content/AssetImporterContext.h"

#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Bruno
{
	class TextureImporter : public AbstractAssetImporter
	{
	public:
		TextureImporter();
		bool TryImport(const AssetMetadata& metadata, AssetImporterContext& context, std::shared_ptr<Asset>& asset) override;

	private:
		
	};
}

