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
}