#include "ContentWriter.h"

#include "ContentTypeWriterManager.h"

namespace Bruno
{
	ContentWriter::ContentWriter(ContentCompiler* compiler, Stream& stream, bool compressContent, const std::wstring& rootDirectory, const std::wstring& referenceRelocationPath) :
		m_finalOutputStream(stream)
	{
		m_currentStream = &m_contentDataStream;
	}

	void ContentWriter::FlushOutput()
	{
		WriteHeader();
		WriteFinalOutput();
	}

	void ContentWriter::WriteBytes(uint8_t* buffer, int count)
	{
		m_currentStream->Write(buffer, count);
	}

	void ContentWriter::WriteChar(char value)
	{
		uint8_t buffer[1];
		buffer[0] = (uint8_t)value;

		m_currentStream->Write(buffer, 1);
	}

	void ContentWriter::WriteInt32(int32_t value)
	{
		uint8_t buffer[4];
		buffer[0] = (uint8_t)value;
		buffer[1] = (uint8_t)(value >> 8);
		buffer[2] = (uint8_t)(value >> 0x10);
		buffer[3] = (uint8_t)(value >> 0x18);

		m_currentStream->Write(buffer, 4);
	}

	void ContentWriter::WriteUInt32(uint32_t value)
	{
		uint8_t buffer[4];
		buffer[0] = (uint8_t)value;
		buffer[1] = (uint8_t)(value >> 8);
		buffer[2] = (uint8_t)(value >> 0x10);
		buffer[3] = (uint8_t)(value >> 0x18);

		m_currentStream->Write(buffer, 4);
	}

	void ContentWriter::WriteObject(const ContentItem& object)
	{
		int typeIndex = -1;
		auto rttiType = object.GetTypeIdInstance();
		AbstractContentTypeWriter* typeWriter = GetTypeWriter(rttiType, typeIndex);

		if (typeIndex < 0)
		{
			throw std::exception("Processor not found");
		}

		WriteInt32(typeIndex + 1);

		typeWriter->Write(*this, object);
	}

	AbstractContentTypeWriter* ContentWriter::GetTypeWriter(RTTI::IdType writerTypeId, int& typeIndex)
	{
		auto it = m_writerIndexTable.find(writerTypeId);
		if (it == m_writerIndexTable.end()) {

			typeIndex = m_writers.size();
			m_writerIndexTable[writerTypeId] = typeIndex;
			auto allTypeWriters = ContentTypeWriterManager::GetContentTypeWriters();
			auto typeWriter = allTypeWriters[writerTypeId];
			
			return typeWriter.get();
		}

		typeIndex = it->second;
		return m_writers[typeIndex];
	}

	void ContentWriter::WriteHeader()
	{
		m_currentStream = &m_headerDataStream;

		WriteUInt32(m_writers.size());
		for (size_t i = 0; i < m_writers.size(); i++)
		{

		}
	}

	void ContentWriter::WriteFinalOutput()
	{
		m_currentStream = &m_finalOutputStream;

		WriteChar('B');
		WriteChar('R');
		WriteChar('U');
		WriteChar('N');
		WriteChar('O');

		m_currentStream->Write(m_headerDataStream.GetBuffer(), m_headerDataStream.GetLength());
		m_currentStream->Write(m_contentDataStream.GetBuffer(), m_contentDataStream.GetLength());
	}
}