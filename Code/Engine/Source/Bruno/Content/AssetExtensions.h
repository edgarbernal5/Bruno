#pragma once

#include "AssetTypes.h"
#include <unordered_map>
#include <string>

namespace Bruno
{
	inline static std::unordered_map<std::string, AssetType> g_AssetExtensionMap =
	{
		{ ".fbx", AssetType::Model },
		{ ".x", AssetType::Model },
		{ ".obj", AssetType::Model },

		{ ".png", AssetType::Texture },
		{ ".jpg", AssetType::Texture },
		{ ".jpeg", AssetType::Texture }
	};
}