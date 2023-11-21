#include "MaterialProcessor.h"

#include "Pipeline/Graphics/MaterialContentItem.h"

#include <filesystem>

namespace Bruno
{
	BR_RTTI_DEFINITIONS(MaterialProcessor);

	MaterialProcessor::MaterialProcessor()
	{
		m_supportedExtensions = { L".mat" };
	}

	std::shared_ptr<ContentItem> MaterialProcessor::Process(const std::wstring& assetFilename, ContentProcessorContext& context)
	{


		return nullptr;
	}
}