#pragma once

#include <string>
#include "Stream.h"
#include "MemoryStream.h"
#include "Pipeline/AbstractProcessor.h"

namespace Bruno
{
	class ContentCompiler;

	class ContentWriter
	{
	public:
		ContentWriter(ContentCompiler* compiler, Stream& stream, bool compressContent, const std::wstring& rootDirectory, std::wstring referenceRelocationPath);

		void WriteObject(const ContentItem& object);

	private:
		Stream& m_stream;
		MemoryStream m_headerDataStream;
		MemoryStream m_contentDataStream;

		Stream* m_currentStream;
	};
}

