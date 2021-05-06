#include "stdafx.h"
#include "Path.h"

#include <algorithm>
#include <filesystem>

namespace TrioIO
{
	bool Path::CreateFolder(const std::string& path)
	{
		std::error_code errorCode;
		bool result = std::filesystem::create_directories(path, errorCode);
		return result;
	}

	std::string Path::Combine(const std::string& p1, const std::string& p2)
	{
		std::string tmp = p1;

		if (p1.empty())
		{
			if (!p2.empty() && p2[0] == DirectorySeparator)
				return p2.substr(1, p2.size());
			return p2;
		}

		if (p1[p1.length() - 1] != DirectorySeparator) {
			tmp += DirectorySeparator;
			if (p2[0] == DirectorySeparator)
				return (tmp + p2.substr(1, p2.size()));
			return(tmp + p2);
		}
		else
		{
			if (p2[0] == DirectorySeparator)
				return (tmp + p2.substr(1, p2.size()));
		}
		return(p1 + p2);
	}
	
	std::string Path::GetDirectoryFromFilePath(std::string path)
	{
		char separator[] = { '\\' };
		std::string::iterator it = find_end(path.begin(), path.end(), separator, separator + 1);

		if (it == path.end())
		{
			return "";
		}
		else
		{
			return std::string(path.begin(), it);
		}
	}

	std::string Path::GetFileExtension(std::string path)
	{
		std::size_t index = path.find_last_of(".");
		if (index != std::string::npos)
		{
			return path.substr(index + 1);
		}
		return path;
	}
	std::string Path::GetFilenameWithoutExtension(std::string path)
	{
		char separator[] = { '\\' };
		size_t it = path.find_last_of("\\");

		if (it == std::string::npos)
		{
			return "";
		}
		else
		{
			std::string file = path.substr(it + 1);
			return file.substr(0, file.size() - 1 - GetFileExtension(file).size());
		}
	}

	std::string Path::GetFullDirectory(const std::string& path)
	{
		char full[1000];
		_fullpath(full, path.c_str(), 1000);

		return std::string(full);
	}

	bool Path::IsPathRooted(std::string path)
	{
		if (path.size() == 0)
			return false;

		if (path[0] == '\\')
			return true;

		if (path.size() >= 2 && path[1] == ':')
			return true;

		return false;
	}
}