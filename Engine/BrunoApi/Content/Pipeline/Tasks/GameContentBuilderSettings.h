#pragma once

#include "TrioApiRequisites.h"

#include "IO/Path.h"

namespace BrunoEngine
{
	struct BRUNO_API_EXPORT GameContentBuilderSettings
	{
		inline std::string& GetIntermediateDirectory()
		{
			return m_intermediateDirectory;
		}

		inline std::string& GetOutputDirectory()
		{
			return m_outputDirectory;
		}

		inline std::string& GetRootDirectory()
		{
			return m_rootDirectory;
		}

		void SetIntermediateDirectory(std::string directory) {
			m_intermediateDirectory = directory;
		}

		void SetOutputDirectory(std::string directory) {
			m_outputDirectory = directory;
		}

		void SetRootDirectory(std::string directory) {
			m_rootDirectory = directory;
		}

		GameContentBuilderSettings()
		{
		}

		std::string GetCacheFilename()
		{
			return m_intermediateDirectory + "Content-Pipeline.xml";
		}

		void InitializePaths()
		{
			if (m_rootDirectory.size() == 0)
			{
				m_rootDirectory = ".";
			}
			m_rootDirectory = BrunoIO::Path::GetFullDirectory(m_rootDirectory + BrunoIO::Path::DirectorySeparator);
			m_intermediateDirectory = PrepareOutputDirectory(m_intermediateDirectory, "tmpObjects");
			m_outputDirectory = PrepareOutputDirectory(m_outputDirectory, "finalObjects");
		}

		std::string PrepareOutputDirectory(std::string directory, std::string defaultValue)
		{
			if (directory.empty())
			{
				directory = BrunoIO::Path::Combine(m_rootDirectory, defaultValue)
					+ BrunoIO::Path::DirectorySeparator;
			}
			else
			{
				directory = BrunoIO::Path::GetFullDirectory(directory + BrunoIO::Path::DirectorySeparator);
			}
			BrunoIO::Path::CreateFolder(directory);
			return directory;
		}

		std::string GetAbsolutePath(std::string path)
		{
			if (path.size() >= 2 && path[1] == ':')
				return path;

			return BrunoIO::Path::Combine(m_rootDirectory, path);
		}

		std::string GetRelativePath(std::string path)
		{
			if (path.size() < m_rootDirectory.size())
			{
				return path;
			}
			else if (path.substr(0, m_rootDirectory.size()) != m_rootDirectory)
			{
				return path;
			}
			return path.substr(m_rootDirectory.size());
		}

	private:
		std::string m_intermediateDirectory;
		std::string m_outputDirectory;
		std::string m_rootDirectory;

	};
}