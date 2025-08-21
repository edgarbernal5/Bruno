#pragma once

#include <Berta/Controls/TreeBox.h>
#include <filesystem>

namespace Bruno
{
	struct ContentBrowserItem
	{
		std::filesystem::directory_entry DirectoryEntry;
		bool IsDirectory;
		Berta::TreeBoxItem TreeNode;
	};
}