#pragma once

#include "TrioApiRequisites.h"

#include "IO/Path.h"
#include "Content/Pipeline/OpaqueData.h"

namespace TrioEngine
{
	struct BuildCoordinatorSettings
	{
		std::string RootDirectory;
		std::string OutputDirectory;
		std::string IntermediateDirectory;

		BuildCoordinatorSettings()
		{
			RootDirectory = "";
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

	class BuildItem;
	class BuildItemCollection;
	class BuildRequest;
	class ContentItem;
	class ContentCompiler;
	class ImporterManager;
	class ProcessorManager;

	class TRIO_API_EXPORT BuildCoordinator
	{
	public:
		BuildCoordinator();
		~BuildCoordinator();

		inline ProcessorManager* GetProcessorManager()
		{
			return m_processorManager;
		}

		inline std::string GetRelativePath(std::string path)
		{
			return m_Settings.GetRelativePath(path);
		}

		BuildCoordinatorSettings& GetBuildSettings()
		{
			return m_Settings;
		}

		void RequestBuild(std::string sourceFilename, std::string assetName, std::string importerName, std::string processorName, OpaqueData *processorParameters);
		BuildItem* RequestBuild(BuildRequest *request);

		std::string GetAbsolutePath(std::string path);
		void RunTheBuild();
	private:
		ContentItem* BuildAsset(BuildItem * item);
		ContentItem* ImportAsset(BuildItem * item);
		std::string ChooseOutputFilename(BuildRequest *request);
		void RemoveBuildItem(BuildItem* item);
		void SerializeAsset(BuildItem *item, ContentItem* assetObject);

		bool m_BuildItemsChanged;

		std::vector<std::string> m_RebuiltFiles;

		ImporterManager *m_importerManager;
		ProcessorManager *m_processorManager;

		ContentCompiler *m_ContentCompiler;

		BuildCoordinatorSettings m_Settings;
		BuildItemCollection *m_BuildItems;
	};
}