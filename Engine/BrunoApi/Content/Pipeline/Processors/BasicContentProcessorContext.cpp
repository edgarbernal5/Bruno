#include "stdafx.h"
#include "BasicContentProcessorContext.h"

#include "Content/Pipeline/Tasks/BuildItem.h"
#include "Content/Pipeline/Tasks/BuildCoordinator.h"
#include "Content/Pipeline/Tasks/BuildRequest.h"
#include "Content/Pipeline/Processors/ProcessorManager.h"
#include "Content/Pipeline/Processors/IContentProcessor.h"

namespace BrunoEngine
{
	BasicContentProcessorContext::BasicContentProcessorContext(BuildCoordinator *buildCoordinator, BuildItem *buildItem, std::string buildConfiguration) :
		m_buildCoordinator(buildCoordinator),
		m_buildItem(buildItem),
		m_buildConfiguration(buildConfiguration)
	{
	}

	std::string BasicContentProcessorContext::GetIntermediateDirectory()
	{
		return "";
	}

	std::string BasicContentProcessorContext::GetOutputDirectory()
	{
		return "";
	}

	OpaqueData* BasicContentProcessorContext::GetParameters()
	{
		return m_opaqueData;
	}

	void BasicContentProcessorContext::AddDependency(std::string filename)
	{
	}

	void BasicContentProcessorContext::AddOutputFile(std::string filename)
	{
	}

	BuildItem* BasicContentProcessorContext::RequestBuild(ExternalReference sourceAsset, std::string assetName, std::string importerName, std::string processorName, OpaqueData* processorParameters)
	{
		BuildRequest* request = new BuildRequest();
		request->m_sourceFileName = sourceAsset.GetFilename();
		request->m_assetName = assetName;
		request->m_importerName = importerName;
		request->m_processorName = processorName;

		if (processorParameters != nullptr)
		{
			request->m_opaqueData = new OpaqueData(*processorParameters);
		}
		return m_buildCoordinator->RequestBuild(request);
	}

	ContentItem* BasicContentProcessorContext::Convert(ContentItem* input, std::string processorName, OpaqueData *processorParameters)
	{
		ProcessorManager* processorManager = m_buildCoordinator->GetProcessorManager();
		IContentProcessor *processor = processorManager->GetByProcessorName(processorName, processorParameters);
		return (processor->Process(input, this));
	}

	ExternalReference BasicContentProcessorContext::BuildAsset(ExternalReference sourceAsset, std::string processorName, OpaqueData *processorParameters, std::string importerName, std::string assetName)
	{
		BuildItem* item = RequestBuild(sourceAsset, assetName, importerName, processorName, processorParameters);

		std::vector<std::string>::iterator itv = find(item->m_requests.begin(), item->m_requests.end(), item->m_outputFilename);
		if (itv == item->m_requests.end())
			item->m_requests.push_back(item->m_outputFilename);

		return ExternalReference(m_buildCoordinator->GetAbsolutePath(item->m_outputFilename));
	}
}