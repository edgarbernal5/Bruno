#pragma once

#include "TrioApiRequisites.h"

#include "ContentProcessorContext.h"

namespace TrioEngine
{
	class BuildItem;
	class BuildCoordinator;
	class BuildRequest;

	class TRIO_API_EXPORT BasicContentProcessorContext : public ContentProcessorContext
	{
	public:
		BasicContentProcessorContext(BuildCoordinator *buildCoordinator, BuildItem *buildItem, std::string buildConfiguration);

		void AddDependency(std::string filename);
		void AddOutputFile(std::string filename);
		ExternalReference BuildAsset(ExternalReference sourceAsset, std::string processorName, OpaqueData* processorParameters, std::string importerName, std::string assetName);

		std::string GetIntermediateDirectory();
		std::string GetOutputDirectory();

		OpaqueData* GetParameters();

		ContentItem* Convert(ContentItem* input, std::string processorName, OpaqueData *processorParameters);
	private:
		OpaqueData* m_opaqueData;

		BuildCoordinator * m_buildCoordinator;
		BuildItem * m_buildItem;
		std::string m_buildConfiguration;

		BuildItem* RequestBuild(ExternalReference sourceAsset, std::string assetName, std::string importerName, std::string processorName, OpaqueData* processorParameters);
	};
}