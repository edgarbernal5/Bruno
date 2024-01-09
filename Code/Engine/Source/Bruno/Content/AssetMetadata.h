#pragma once

#include "Asset.h"
#include <string>

namespace Bruno
{
	struct AssetMetadata
	{
		AssetHandle Handle{ 0 };
		std::wstring Filename;

		operator bool() const { return Handle != 0; }
	};
}