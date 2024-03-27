#pragma once

#include "Asset.h"
#include "AssetTypes.h"
#include <string>

namespace Bruno
{
	struct AssetMetadata
	{
		AssetHandle Handle{ 0 };
		AssetType Type{ AssetType::None };
		std::wstring Filename;
		bool IsLoaded{ false };
		bool IsMemoryOnly{ false };

		operator bool() const { return Handle != 0; }
	};
}