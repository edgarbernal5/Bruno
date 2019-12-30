#pragma once

#include "TrioApiRequisites.h"

#include <string>

namespace TrioIO
{
	class Path
	{
	public:
		static bool CreateFolder(const std::string& path);
		static std::string Combine(const std::string& p1, const std::string& p2);
		static std::string GetDirectoryFromFilePath(std::string path);
		static std::string GetFileExtension(std::string path);
		static std::string GetFilenameWithoutExtension(std::string path);
		static std::string GetFullDirectory(const std::string& path);
		static bool IsPathRooted(std::string path);
	};
}