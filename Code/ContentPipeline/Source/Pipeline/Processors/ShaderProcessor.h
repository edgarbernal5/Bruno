#pragma once

#include "Pipeline/AbstractProcessor.h"

namespace Bruno
{
	class ShaderProcessor : public AbstractProcessor
	{
		BR_RTTI_DECLARATION(ShaderProcessor, AbstractProcessor);

	public:
		ShaderProcessor();

		virtual std::shared_ptr<ContentItem> Process(const std::wstring& assetFilename, ContentProcessorContext& context) override;
	};
}
