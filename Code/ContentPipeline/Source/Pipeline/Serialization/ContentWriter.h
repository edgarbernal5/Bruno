#pragma once

#include <string>
#include <map>
#include <vector>

#include <Bruno/Core/MemoryStream.h>
#include "Pipeline/AbstractProcessor.h"

namespace Bruno
{
	class ContentCompiler;
	class AbstractContentTypeWriter;

	class ContentWriter
	{
	public:
		ContentWriter(ContentCompiler* compiler, Stream& stream, bool compressContent, const std::wstring& rootDirectory, const std::wstring& referenceRelocationPath);

		void FlushOutput();

		void WriteBytes(const std::vector<uint8_t>& buffer);
		void WriteChar(char value);
		void WriteInt32(int32_t value);
		void WriteUInt32(uint32_t value);
		void WriteObject(const ContentItem& object);
		void WriteString(const std::string& value);
		
	private:
		Stream& m_finalOutputStream;
		MemoryStream m_headerDataStream;
		MemoryStream m_contentDataStream;

		Stream* m_currentStream;

		std::map<RTTI::IdType, int> m_writersIndexTable;
		std::vector<AbstractContentTypeWriter*> m_writers;

		AbstractContentTypeWriter* GetTypeWriter(RTTI::IdType writerTypeId, int& typeIndex);
		void WriteHeader();
		void WriteFinalOutput();
	};
}

