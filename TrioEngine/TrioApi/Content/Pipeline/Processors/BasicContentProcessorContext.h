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

		std::string GetIntermediateDirectory();
		std::string GetOutputDirectory();

		OpaqueData* GetParameters();

		void AddDependency(std::string filename);
		void AddOutputFile(std::string filename);

		ExternalReference BuildAsset(ExternalReference sourceAsset, std::string processorName, OpaqueData* processorParameters, std::string importerName, std::string assetName);

		ContentItem* Convert(ContentItem* input, std::string processorName, OpaqueData *processorParameters);
	private:
		OpaqueData* m_OpaqueData;

		BuildCoordinator * m_BuildCoordinator;
		BuildItem * m_BuildItem;
		std::string m_BuildConfiguration;

		BuildItem* RequestBuild(ExternalReference sourceAsset, std::string assetName, std::string importerName, std::string processorName, OpaqueData* processorParameters);
	};
}