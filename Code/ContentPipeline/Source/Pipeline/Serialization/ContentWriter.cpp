#include "ContentWriter.h"

#include "ContentTypeWriterManager.h"
#include <filesystem>

#include "Bruno/Core/Compressor.h"
#include <Bruno/Core/Log.h>

namespace Bruno
{
	ContentWriter::ContentWriter(ContentCompiler* compiler, Stream& stream, bool compressContent, const std::wstring& rootDirectory, const std::wstring& referenceRelocationPath) :
		m_finalOutputStream(stream),
		m_compressContent(compressContent),
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

	void ContentWriter::WriteExternalReference(const ExternalReferenceContentItem& reference)
	{
		if (reference.Filename.empty())
		{
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
			if (outputPath.size() > 3 && outputPath.substr(0, 3) == L"..\\")
			{
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
				uint32_t resourceIndex = static_cast<uint32_t>(m_sharedResources.size());
				m_sharedResourcesIndexTable[resource] = resourceIndex;
				WriteUInt32(resourceIndex);
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

	void ContentWriter::WriteVector2(const Math::Vector2& vector2)
	{
		m_currentStream->WriteRaw<Math::Vector2>(vector2);
	}

	void ContentWriter::WriteVector3(const Math::Vector3& vector3)
	{
		m_currentStream->WriteRaw<Math::Vector3>(vector3);
	}

	void ContentWriter::WriteVector4(const Math::Vector4& vector4)
	{
		m_currentStream->WriteRaw<Math::Vector4>(vector4);
	}

	AbstractContentTypeWriter* ContentWriter::GetTypeWriter(RTTI::IdType writerTypeId, int& typeIndex)
	{
		auto it = m_writersIndexTable.find(writerTypeId);
		if (it == m_writersIndexTable.end()) {

			typeIndex = static_cast<int>(m_writers.size());
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

		const int BrunoHeaderSize = 5; // "BRUNO"

		if (m_compressContent)
		{
			WriteChar('1'); // compression
			WriteInt32(0); // we don't know total length, this will be filled after compression.
			WriteInt32(headerLength + contentLength);

			Compressor compressor(m_finalOutputStream);
			compressor.Compress(m_headerDataStream.GetBuffer().Data, static_cast<unsigned long>(m_headerDataStream.GetLength()));
			compressor.Compress(m_contentDataStream.GetBuffer().Data, static_cast<unsigned long>(m_contentDataStream.GetLength()));
			compressor.FlushOutput();

			int compressedHeaderAndContentLength = (int)m_finalOutputStream.GetLength();
			int uncompressedHeaderAndContentLength = BrunoHeaderSize + sizeof(char) + sizeof(int) + headerLength + contentLength;

			if (compressedHeaderAndContentLength < uncompressedHeaderAndContentLength)
			{
				m_currentStream->SetPosition(6); // put position right after prologue (BRUNO + flag compression)
				WriteInt32(compressedHeaderAndContentLength);
				return;
			}
			m_currentStream->SetPosition(4);
		}

		WriteChar('0'); // no compression
		WriteInt32(BrunoHeaderSize + sizeof(char) + sizeof(int) + headerLength + contentLength);
		m_currentStream->Write(m_headerDataStream.GetBuffer().Data, m_headerDataStream.GetLength());
		m_currentStream->Write(m_contentDataStream.GetBuffer().Data, m_contentDataStream.GetLength());
	}
}