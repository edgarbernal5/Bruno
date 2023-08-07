#include "ContentProcessorContext.h"

#include "GameContentBuilder.h"
#include "Pipeline/ProcessorManager.h"

namespace Bruno
{
	ContentProcessorContext::ContentProcessorContext(GameContentBuilder* builder, BuildItem* buildItem) :
		m_gameContentBuilder(builder),
		m_currentBuildItem(buildItem)
	{
	}
	std::shared_ptr<ContentItem> ContentProcessorContext::Convert(ContentItem* item, const std::string& processorName)
	{
		auto processor = ProcessorManager::GetProcessorByName(processorName);

		//return processor->Process(;
		return nullptr;
	}
}