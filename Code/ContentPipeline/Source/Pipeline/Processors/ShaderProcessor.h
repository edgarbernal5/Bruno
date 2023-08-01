#pragma once

#include "Pipeline/AbstractProcessor.h"

namespace Bruno
{
	class ShaderProcessor : public AbstractProcessor
	{
		BR_RTTI_DECLARATION(ShaderProcessor, AbstractProcessor);

	public:
		ShaderProcessor();

		virtual ContentItem* Process(const std::wstring& assetFilename) override;
	};
}
