#pragma once

#include "Bruno/Content/Asset.h"
#include <string>
#include <map>

namespace Bruno
{
	class Texture;

	class Material : public Asset
	{
		BR_RTTI_DECLARATION(Material, Asset);

	public:
		Material();

		AssetType GetAssetType() const override { return AssetType::Material; }

		std::string Name;
		std::map<std::string, AssetHandle> TexturesByName;
		//std::map<std::string, std::shared_ptr<Texture>> TexturesByName;
	};
}
