#pragma once

#include "Pipeline/AbstractProcessor.h"

namespace Bruno
{
	class TextureProcessor : public AbstractProcessor
	{
		BR_RTTI_DECLARATION(TextureProcessor, AbstractProcessor);

	public:
		TextureProcessor();

		virtual ContentItem Process(const std::wstring& assetFilename) override;
	};
}
