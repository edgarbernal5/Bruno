#include "ContentWriter.h"

#include "ContentTypeWriterManager.h"
#include <filesystem>

namespace Bruno
{
	ContentWriter::ContentWriter(ContentCompiler* compiler, Stream& stream, bool compressContent, const std::wstring& rootDirectory, const std::wstring& referenceRelocationPath) :
		m_finalOutputStream(stream),
		m_referenceRelocationPath(referenceRelocationPath)
	{
		m_currentStream = &m_contentDataStream;
	}

	void ContentWriter::FlushOutput()
	{
		WriteSharedResources();
		WriteHeader();
		WriteFinalOutput();
	}

	void ContentWriter::WriteBytes(const std::vector<uint8_t>& buffer)
	{
		m_currentStream->WriteBytes(buffer);
	}

	void ContentWriter::WriteChar(char value)
	{
		uint8_t buffer[1];
		buffer[0] = (uint8_t)value;

		m_currentStream->Write(buffer, 1);
	}

	void ContentWriter::WriteExternalReference(ExternalReferenceContentItem& const reference)
	{
		if (reference.Filename.empty()) {
			WriteWString(L"");
		}
		else
		{
			std::wstring filename = reference.Filename;
			if (std::filesystem::path(filename).extension() != FileExtension)
			{
				throw std::exception("external reference. ext");
			}
			filename = filename.substr(0, filename.size() - wcslen(FileExtension));
			std::filesystem::path basePath(m_referenceRelocationPath);

			auto outputPath = std::filesystem::relative(filename, basePath).wstring();
			if (outputPath.size() > 3 && outputPath.substr(0, 3) == L"..\\") {
				outputPath = outputPath.substr(3);
			}
			WriteWString(outputPath);
		}
	}

	void ContentWriter::WriteInt32(int32_t value)
	{
		/*uint8_t buffer[4];
		buffer[0] = (uint8_t)value;
		buffer[1] = (uint8_t)(value >> 8);
		buffer[2] = (uint8_t)(value >> 0x10);
		buffer[3] = (uint8_t)(value >> 0x18);

		m_currentStream->Write(buffer, 4);*/

		m_currentStream->WriteRaw<int32_t>(value);
	}

	void ContentWriter::WriteInt64(int64_t value)
	{
		m_currentStream->WriteRaw<int64_t>(value);
	}

	void ContentWriter::WriteUInt8(uint8_t value)
	{
		m_currentStream->WriteRaw<uint8_t>(value);
	}

	void ContentWriter::WriteUInt32(uint32_t value)
	{
		m_currentStream->WriteRaw<uint32_t>(value);
	}

	void ContentWriter::WriteUInt64(uint64_t value)
	{
		m_currentStream->WriteRaw<uint64_t>(value);
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

	void ContentWriter::WriteString(const std::string& value)
	{
		m_currentStream->WriteString(value);
	}

	void ContentWriter::WriteWString(const std::wstring& value)
	{
		m_currentStream->WriteWString(value);
	}

	void ContentWriter::WriteSharedResource(const ContentItem* resource)
	{
		if (resource == nullptr) {
			WriteUInt32(0);
		}
		else
		{
			auto it = m_sharedResourcesIndexTable.find(resource);
			if (it == m_sharedResourcesIndexTable.end())
			{
				m_sharedResources.push(resource);
				m_sharedResourcesIndexTable[resource] = m_sharedResources.size();
				WriteUInt32(m_sharedResources.size());
			}
			else
			{
				WriteUInt32(it->second);
			}
		}
	}

	void ContentWriter::WriteSharedResources()
	{
		while (m_sharedResources.size() > 0)
		{
			auto resource = m_sharedResources.front(); m_sharedResources.pop();
			WriteObject(*resource);
		}
	}

	AbstractContentTypeWriter* ContentWriter::GetTypeWriter(RTTI::IdType writerTypeId, int& typeIndex)
	{
		auto it = m_writersIndexTable.find(writerTypeId);
		if (it == m_writersIndexTable.end()) {

			typeIndex = m_writers.size();
			m_writersIndexTable[writerTypeId] = typeIndex;
			auto allTypeWriters = ContentTypeWriterManager::GetContentTypeWriters();
			auto typeWriter = allTypeWriters[writerTypeId];
			
			m_writers.push_back(typeWriter.get());
			return typeWriter.get();
		}

		typeIndex = it->second;
		return m_writers[typeIndex];
	}

	void ContentWriter::WriteHeader()
	{
		m_currentStream = &m_headerDataStream;

		WriteUInt64(m_writers.size());
		for (size_t i = 0; i < m_writers.size(); i++)
		{
			WriteString(m_writers[i]->GetReaderName());
		}
		WriteUInt32((uint32_t)m_sharedResourcesIndexTable.size());
	}

	void ContentWriter::WriteFinalOutput()
	{
		m_currentStream = &m_finalOutputStream;

		WriteChar('B');
		WriteChar('R');
		WriteChar('U');
		WriteChar('N');
		WriteChar('O');

		int headerLength = (int)m_headerDataStream.GetLength();
		int contentLength = (int)m_contentDataStream.GetLength();

		const int HeaderSize = 5; // "BRUNO"

		WriteInt32(HeaderSize + sizeof(int) + headerLength + contentLength);
		m_currentStream->Write(m_headerDataStream.GetBuffer(), m_headerDataStream.GetLength());
		m_currentStream->Write(m_contentDataStream.GetBuffer(), m_contentDataStream.GetLength());
	}
}