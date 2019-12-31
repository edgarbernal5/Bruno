#pragma once

#include "TrioApiRequisites.h"

#include "Content/Pipeline/OpaqueData.h"

#include "BuildCoordinatorSettings.h"

namespace TrioEngine
{
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
		BuildCoordinator(BuildCoordinatorSettings settings);
		~BuildCoordinator();

		std::string GetAbsolutePath(std::string path);

		BuildCoordinatorSettings& GetBuildSettings()
		{
			return m_Settings;
		}

		inline ProcessorManager* GetProcessorManager()
		{
			return m_processorManager;
		}

		inline std::string GetRelativePath(std::string path)
		{
			return m_Settings.GetRelativePath(path);
		}

		void RequestBuild(std::string sourceFilename, std::string assetName, std::string importerName, std::string processorName, OpaqueData *processorParameters);
		BuildItem* RequestBuild(BuildRequest *request);
		void RunTheBuild();
	private:
		ContentItem* BuildAsset(BuildItem * item);
		std::string ChooseOutputFilename(BuildRequest *request);
		ContentItem* ImportAsset(BuildItem * item);
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