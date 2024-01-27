#pragma once

#include <nana/gui/widgets/treebox.hpp>
#include <filesystem>

namespace Bruno
{
	struct ContentBrowserItem
	{
		std::filesystem::directory_entry DirectoryEntry;
		bool IsDirectory;
		nana::treebox::item_proxy TreeNode;
	};
}