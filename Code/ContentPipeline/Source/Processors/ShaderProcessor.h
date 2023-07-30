#pragma once

#include "AbstractProcessor.h"

namespace Bruno
{
	class ShaderProcessor : public AbstractProcessor
	{
		BR_RTTI_DECLARATION(ShaderProcessor, AbstractProcessor);
	public:
		ShaderProcessor();

		virtual void Process(const std::wstring& assetFilename) override;
	};
}
