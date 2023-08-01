#pragma once

#include "Pipeline/AbstractProcessor.h"

namespace Bruno
{
	class TextureProcessor : public AbstractProcessor
	{
		BR_RTTI_DECLARATION(TextureProcessor, AbstractProcessor);

	public:
		TextureProcessor();

		virtual std::shared_ptr<ContentItem> Process(const std::wstring& assetFilename) override;
	};
}
