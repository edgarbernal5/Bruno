#pragma once

#include "TrioApiRequisites.h"

#include "Content/Pipeline/OpaqueData.h"

#include "BuildCoordinatorSettings.h"
#include "Content/Pipeline/Tasks/TimestampCache.h"

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
		BuildCoordinator(BuildCoordinatorSettings settings, TimestampCache* timestampCache);
		~BuildCoordinator();

		void AddDependency(BuildItem* buildItem, std::string filename);
		std::string GetAbsolutePath(std::string path);

		BuildCoordinatorSettings& GetBuildSettings()
		{
			return m_settings;
		}

		inline ProcessorManager* GetProcessorManager()
		{
			return m_processorManager;
		}

		inline std::string GetRelativePath(std::string path)
		{
			return m_settings.GetRelativePath(path);
		}

		void RequestBuild(std::string sourceFilename, std::string assetName, std::string importerName, std::string processorName, OpaqueData *processorParameters);
		BuildItem* RequestBuild(BuildRequest *request);
		void RunTheBuild();

	private:
		ContentItem* BuildAsset(BuildItem * item);
		std::string ChooseOutputFilename(BuildRequest *request);
		std::string GetAssetNameStub(std::string filename);
		ContentItem* ImportAsset(BuildItem * item);
		void RemoveBuildItem(BuildItem* item);
		void SerializeAsset(BuildItem *item, ContentItem* assetObject);

		bool m_buildItemsChanged;

		std::vector<std::string> m_rebuiltFiles;

		ImporterManager *m_importerManager;
		ProcessorManager *m_processorManager;

		ContentCompiler *m_contentCompiler;

		BuildCoordinatorSettings m_settings;
		BuildItemCollection *m_buildItems;
		TimestampCache m_timestampCache;
	};
}