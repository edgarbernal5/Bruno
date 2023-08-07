#pragma once

#include "Pipeline/AbstractProcessor.h"

namespace Bruno
{
	class MaterialProcessor : public AbstractProcessor
	{
		BR_RTTI_DECLARATION(MaterialProcessor, AbstractProcessor);

	public:
		MaterialProcessor();

		virtual std::shared_ptr<ContentItem> Process(const std::wstring& assetFilename, ContentProcessorContext& context) override;
	};
}
