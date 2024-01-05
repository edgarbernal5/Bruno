#pragma once

#include "Asset.h"
#include <filesystem>

namespace Bruno
{
	struct AssetMetadata
	{
		AssetHandle Handle{ 0 };
		std::filesystem::path Filename;
	};
}