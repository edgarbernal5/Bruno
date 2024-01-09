#pragma once

#include <cstdint>

namespace Bruno
{
	enum class AssetType : uint16_t
	{
		None = 0,
		Model,
		Texture
	};

	inline std::string GetAssetTypeStringFrom(AssetType type)
	{
		switch (type)
		{
		case Bruno::AssetType::None:
			return "None";
		case Bruno::AssetType::Model:
			return "Model";
		case Bruno::AssetType::Texture:
			return "Texture";
		default:
			return "Unknown";
		}
	}
}