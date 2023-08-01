#pragma once

#include "Stream.h"
#include "Pipeline/AbstractProcessor.h"

namespace Bruno
{
	class ContentCompiler
	{
	public:
		ContentCompiler() = default;

		void Compile(Stream& stream, const ContentItem& object, bool compressContent, const std::wstring& rootDirectory, const std::wstring& referenceRelocationPath);

	};
}

