#include "stdafx.h"
#include "BasicContentProcessorContext.h"

#include "Content/Pipeline/Tasks/BuildItem.h"
#include "Content/Pipeline/Tasks/BuildCoordinator.h"
#include "Content/Pipeline/Tasks/BuildRequest.h"
#include "Content/Pipeline/Processors/ProcessorManager.h"
#include "Content/Pipeline/Processors/IContentProcessor.h"

namespace TrioEngine
{
	BasicContentProcessorContext::BasicContentProcessorContext(BuildCoordinator *buildCoordinator, BuildItem *buildItem, std::string buildConfiguration) :
		m_BuildCoordinator(buildCoordinator),
		m_BuildItem(buildItem),
		m_BuildConfiguration(buildConfiguration)
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
		return m_OpaqueData;
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
		request->m_SourceFileName = sourceAsset.GetFilename();
		request->m_AssetName = assetName;
		request->m_ImporterName = importerName;
		request->m_ProcessorName = processorName;

		if (processorParameters != nullptr)
		{
			request->m_OpaqueData = new OpaqueData(*processorParameters);
		}
		return m_BuildCoordinator->RequestBuild(request);
	}

	ContentItem* BasicContentProcessorContext::Convert(ContentItem* input, std::string processorName, OpaqueData *processorParameters)
	{
		ProcessorManager* processorManager = m_BuildCoordinator->GetProcessorManager();
		IContentProcessor *processor = processorManager->GetByProcessorName(processorName, processorParameters);
		return (processor->Process(input, this));
	}

	ExternalReference BasicContentProcessorContext::BuildAsset(ExternalReference sourceAsset, std::string processorName, OpaqueData *processorParameters, std::string importerName, std::string assetName)
	{
		BuildItem* item = RequestBuild(sourceAsset, assetName, importerName, processorName, processorParameters);

		std::vector<std::string>::iterator itv = find(item->m_Requests.begin(), item->m_Requests.end(), item->OutputFilename);
		if (itv == item->m_Requests.end())
			item->m_Requests.push_back(item->OutputFilename);

		return ExternalReference(m_BuildCoordinator->GetAbsolutePath(item->OutputFilename));
	}
}