#pragma once

#include "TrioApiRequisites.h"

#include "ContentProcessorContext.h"

namespace BrunoEngine
{
	class BuildItem;
	class GameContentBuilder;
	class BuildRequest;

	class BRUNO_API_EXPORT BasicContentProcessorContext : public ContentProcessorContext
	{
	public:
		BasicContentProcessorContext(GameContentBuilder *buildCoordinator, BuildItem *buildItem, std::string buildConfiguration);

		void AddDependency(std::string filename);
		void AddOutputFile(std::string filename);
		ExternalReference BuildAsset(ExternalReference sourceAsset, std::string processorName, OpaqueData* processorParameters, std::string importerName, std::string assetName);

		std::string GetIntermediateDirectory();
		std::string GetOutputDirectory();

		OpaqueData* GetParameters();

		ContentItem* Convert(ContentItem* input, std::string processorName, OpaqueData *processorParameters);
	private:
		OpaqueData* m_opaqueData;

		GameContentBuilder * m_buildCoordinator;
		BuildItem * m_buildItem;
		std::string m_buildConfiguration;

		BuildItem* RequestBuild(ExternalReference sourceAsset, std::string assetName, std::string importerName, std::string processorName, OpaqueData* processorParameters);
	};
}