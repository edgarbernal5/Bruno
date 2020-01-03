#pragma once

#include "TrioApiRequisites.h"

#include "IO/Path.h"

namespace TrioEngine
{
	struct TRIO_API_EXPORT BuildCoordinatorSettings
	{
		std::string IntermediateDirectory;
		std::string OutputDirectory;
		std::string RootDirectory;

		BuildCoordinatorSettings()
		{
		}

		std::string GetCacheFilename()
		{
			return IntermediateDirectory + "Content-Pipeline.xml";
		}

		void InitializePaths()
		{
			RootDirectory = TrioIO::Path::GetFullDirectory(RootDirectory);
			IntermediateDirectory = PrepareOutputDirectory(IntermediateDirectory, "obj");
			OutputDirectory = PrepareOutputDirectory(OutputDirectory, "bin");
		}

		std::string PrepareOutputDirectory(std::string directory, std::string defaultValue)
		{
			if (directory.empty())
			{
				directory = TrioIO::Path::Combine(RootDirectory, defaultValue);
			}
			else
			{
				directory = TrioIO::Path::GetFullDirectory(directory);
			}
			TrioIO::Path::CreateFolder(directory);
			return directory;
		}

		std::string GetAbsolutePath(std::string path)
		{
			if (path.size() >= 2 && path[1] == ':')
				return path;

			return TrioIO::Path::Combine(RootDirectory, path);
		}

		std::string GetRelativePath(std::string path)
		{
			if (path.size() < RootDirectory.size())
			{
				return path;
			}
			else if (path.substr(0, RootDirectory.size()) != RootDirectory)
			{
				return path;
			}
			return path.substr(RootDirectory.length());
		}
	};
}