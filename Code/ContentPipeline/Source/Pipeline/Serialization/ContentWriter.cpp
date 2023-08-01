#include "ContentWriter.h"

namespace Bruno
{
	ContentWriter::ContentWriter(ContentCompiler* compiler, Stream& stream, bool compressContent, const std::wstring& rootDirectory, std::wstring referenceRelocationPath) :
		m_stream(stream)
	{
		m_currentStream = &m_contentDataStream;
	}

	void ContentWriter::WriteObject(const ContentItem& object)
	{

	}
}