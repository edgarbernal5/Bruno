#pragma once

#include "AbstractProcessor.h"

namespace Bruno
{
	class TextureProcessor : public AbstractProcessor
	{
		BR_RTTI_DECLARATION(TextureProcessor, AbstractProcessor);
	public:
		TextureProcessor();

		virtual void Process(const std::wstring& assetFilename) override;
	};
}
