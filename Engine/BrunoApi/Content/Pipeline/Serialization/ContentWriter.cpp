#include "stdafx.h"
#include "ContentWriter.h"

#include "IO/Stream.h"
#include "IO/MemoryStream.h"

#include "ContentCompiler.h"
#include "ContentTypeWriter.h"

#include "Errors/ContentPipelineException.h"

#include <filesystem>

namespace BrunoEngine
{
	ContentWriter::ContentWriter(ContentCompiler *compiler, TrioIO::Stream *_stream, bool compressContent, std::string rootDirectory, std::string referenceRelocationPath) :
		TrioIO::BinaryWriter(_stream),
		m_compiler(compiler),
		m_rootDirectory(rootDirectory),
		m_referenceRelocationPath(referenceRelocationPath),
		m_finalOutput(_stream)
	{
		m_headerData = new TrioIO::MemoryStream();
		m_contentData = new TrioIO::MemoryStream();

		m_stream = m_contentData;
	}

	ContentWriter::~ContentWriter()
	{
	}

	void ContentWriter::Write(const Matrix& value)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
				WriteSingle(value.m[i][j]);
		}
	}

	void ContentWriter::Write(const Vector2& value)
	{
		WriteSingle(value.x);
		WriteSingle(value.y);
	}

	void ContentWriter::Write(const Vector3& value)
	{
		WriteSingle(value.x);
		WriteSingle(value.y);
		WriteSingle(value.z);
	}

	void ContentWriter::Write(const Vector4& value)
	{
		WriteSingle(value.x);
		WriteSingle(value.y);
		WriteSingle(value.z);
		WriteSingle(value.w);
	}

	void ContentWriter::FlushOutput()
	{
		WriteSharedResources();
		WriteHeader();
		WriteFinalOutput();
	}

	void ContentWriter::WriteFinalOutput()
	{
		m_stream = m_finalOutput;

		WriteChar('B');
		WriteChar('R');
		WriteChar('U');
		WriteChar('N');
		WriteChar('O');
		WriteByte((uint8_t)0); //plataforma: Windows = 0

		WriteUncompressedOutput();
	}

	void ContentWriter::WriteUncompressedOutput()
	{
		int headerLength = (int)m_headerData->GetLength();
		int contentLength = (int)m_contentData->GetLength();

		WriteInt32((int)(HeaderSize + sizeof(int) + headerLength + contentLength));

		m_stream->Write(m_headerData->GetBuffer(), 0, headerLength);
		m_stream->Write(m_contentData->GetBuffer(), 0, contentLength);
	}

	ContentTypeWriter* ContentWriter::GetTypeWriter(std::string name, int &typeIndex)
	{
		auto itm = m_typeTable.find(name);
		if (itm != m_typeTable.end())
		{
			typeIndex = (*itm).second;
			return m_typeWriters[(*itm).second];
		}

		std::vector<std::string> dependencies;
		ContentTypeWriter* writer = m_compiler->GetTypeWriter(name, dependencies);
		typeIndex = m_typeWriters.size();
		m_typeWriters.push_back(writer);
		m_typeTable[name] = typeIndex;

		for (auto& current : dependencies)
		{
			int num;
			GetTypeWriter(current, num);
		}
		return writer;
	}

	void ContentWriter::BeginWrite()
	{
	}

	void ContentWriter::WriteObject(ContentItem* value)
	{
		if (value == nullptr)
		{
			WriteInt32(0);
			return;
		}

		int typeIndex = -1;
		ContentTypeWriter* typeWriter = nullptr;

		if (value->m_processorName.size() == 0)
			throw ContentPipelineException("Processor name is null");
		
		typeWriter = GetTypeWriter(value->m_processorName, typeIndex);

		WriteInt32(typeIndex + 1);
		auto itm = m_recurseDetector.find(value);
		if (itm != m_recurseDetector.end())
		{
			throw ContentPipelineException("found cyclic reference");
		}
		m_recurseDetector[value] = true;
		typeWriter->Write(this, value);
		m_recurseDetector.erase(value);

	}

	void ContentWriter::WriteHeader()
	{
		m_stream = m_headerData;

		WriteUInt32((uint32_t)m_typeWriters.size());
		for (int i = 0; i < m_typeWriters.size(); i++)
		{
			WriteString(m_typeWriters[i]->GetReaderName());
		}
		WriteUInt32((uint32_t)m_sharedResourceNames.size());
	}

	void ContentWriter::WriteSharedResources()
	{
		while (m_sharedResources.size() > 0)
		{
			ContentItem* sq = m_sharedResources.front(); m_sharedResources.pop();
			WriteObject(sq);
		}
	}

	void ContentWriter::WriteCompressedOutput()
	{
	}

	void ContentWriter::WriteExternalReference(ExternalReference& const reference)
	{
		if (reference.GetFilename().size() == 0)
		{
			WriteString("");
		}
		else
		{
			std::string filename = reference.GetFilename();
			if (filename.empty()) {
				WriteString("");
				return;
			}

			if (TrioIO::Path::GetFileExtension(filename) != FileExtension)
			{
				throw ContentPipelineException("external reference is not a .bruno file");
			}

			filename = filename.substr(0, filename.size() - strlen(FileExtension) - 1);
			if (filename.substr(0, m_rootDirectory.size()) != m_rootDirectory)
			{
				throw ContentPipelineException("external reference with bad path");
			}
			std::filesystem::path basePath(m_referenceRelocationPath);
			std::filesystem::path path(filename);

			auto outputPath = std::filesystem::relative(path, basePath).string();
			if (outputPath.size() > 3 && outputPath.substr(0, 3) == "..\\") {
				outputPath = outputPath.substr(3);
			}
			WriteString(outputPath);
		}
	}

	void ContentWriter::WriteSharedResource(ContentItem * resource)
	{
		if (resource == nullptr)
		{
			WriteInt32(0);
		}
		else
		{
			int resourceIndex = -1;
			//std::map<int, int>::iterator itm = m_SharedResourceNames.find(resource->GetID());
			auto itm = m_sharedResourceNames.find(resource);
			if (itm == m_sharedResourceNames.end())
			{
				resourceIndex = m_sharedResourceNames.size() + 1;
				m_sharedResourceNames[resource] = resourceIndex;
				m_sharedResources.push(resource);
			}
			else
			{
				resourceIndex = (*itm).second;
			}

			WriteInt32(resourceIndex);
		}
	}
}