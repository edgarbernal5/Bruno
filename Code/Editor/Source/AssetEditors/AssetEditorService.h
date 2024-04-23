#pragma once

#include <string>
#include "AssetEditor.h"

namespace Bruno
{
	class AssetEditorService
	{
	public:

		void Open(const std::wstring& filename);
	};
}