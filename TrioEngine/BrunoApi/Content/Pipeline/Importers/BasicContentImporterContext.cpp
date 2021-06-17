#include "stdafx.h"
#include "BasicContentImporterContext.h"

#include "Content/Pipeline/Tasks/BuildItem.h"
#include "Content/Pipeline/Tasks/BuildCoordinator.h"
#include "Content/Pipeline/Tasks/BuildRequest.h"
//#include "Content/Pipeline/Processors/ProcessorManager.h"
//#include "Content/Pipeline/Processors/IContentProcessor.h"

namespace BrunoEngine
{
	BasicContentImporterContext::BasicContentImporterContext(BuildCoordinator *buildCoordinator, BuildItem *buildItem) :
		m_buildCoordinator(buildCoordinator),
		m_buildItem(buildItem)
	{
	}

	void BasicContentImporterContext::AddDependency(std::string filename)
	{
		m_buildCoordinator->AddDependency(m_buildItem, filename);
	}

	std::string BasicContentImporterContext::GetIntermediateDirectory()
	{
		return m_buildCoordinator->GetBuildSettings().GetIntermediateDirectory();
	}

	std::string BasicContentImporterContext::GetOutputDirectory()
	{
		return m_buildCoordinator->GetBuildSettings().GetOutputDirectory();
	}
}