#include "stdafx.h"
#include "ContentWriter.h"

#include "IO/Stream.h"
#include "IO/MemoryStream.h"

#include "ContentCompiler.h"
#include "ContentTypeWriter.h"

namespace TrioEngine
{
	ContentWriter::ContentWriter(ContentCompiler *compiler, TrioIO::Stream *_stream, bool compressContent) :
		TrioIO::BinaryWriter(_stream),
		m_Compiler(compiler),

		m_FinalOutput(_stream)
	{
		m_HeaderData = new TrioIO::MemoryStream();
		m_ContentData = new TrioIO::MemoryStream();

		m_Stream = m_ContentData;
	}

	ContentWriter::~ContentWriter()
	{
	}

	void ContentWriter::Write(Matrix value)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
				WriteSingle(value.m[i][j]);
		}
	}

	void ContentWriter::Write(Vector2 value)
	{
		WriteSingle(value.x);
		WriteSingle(value.y);
	}

	void ContentWriter::Write(Vector3 value)
	{
		WriteSingle(value.x);
		WriteSingle(value.y);
		WriteSingle(value.z);
	}

	void ContentWriter::Write(Vector4 value)
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
		m_Stream = m_FinalOutput;

		WriteChar('T');
		WriteChar('R');
		WriteChar('I');
		WriteChar('O');
		WriteByte((uint8_t)0); //plataforma: Windows

		WriteUncompressedOutput();
	}

	void ContentWriter::WriteUncompressedOutput()
	{
		int length = (int)m_HeaderData->GetLength();
		int count = (int)m_ContentData->GetLength();

		WriteInt32((int)((6 + 4 + length) + count));

		m_Stream->Write(m_HeaderData->GetBuffer(), 0, length);
		m_Stream->Write(m_ContentData->GetBuffer(), 0, count);
	}

	ContentTypeWriter* ContentWriter::GetTypeWriter(std::string name, int &typeIndex)
	{
		std::map<std::string, int>::iterator itm = m_TypeTable.find(name);
		if (itm != m_TypeTable.end())
		{
			typeIndex = (*itm).second;
			return m_TypeWriters[(*itm).second];
		}

		std::vector<std::string> dependencies;
		ContentTypeWriter* writer = m_Compiler->GetTypeWriter(name, dependencies);
		typeIndex = m_TypeWriters.size();
		m_TypeWriters.push_back(writer);
		m_TypeTable[name] = typeIndex;

		for (int i = 0; i < dependencies.size(); i++)
		{
			//
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
		}
		else
		{
			int num = -1;
			ContentTypeWriter *typeWriter = nullptr;

			if (value->m_processorName.size() == 0)
				throw std::exception("Processor name null");
			else
				typeWriter = GetTypeWriter(value->m_processorName, num);

			WriteInt32(num + 1);
			std::map<ContentItem*, bool>::iterator itm = m_recurseDetector.find(value);
			if (itm != m_recurseDetector.end())
			{
				throw std::exception("FoundCyclicReference");
			}
			m_recurseDetector[value] = true;
			typeWriter->Write(this, value);
			m_recurseDetector.erase(value);
		}
	}

	void ContentWriter::WriteHeader()
	{
		m_Stream = m_HeaderData;

		WriteUInt32((uint32_t)m_TypeWriters.size());
		for (int i = 0; i < m_TypeWriters.size(); i++)
		{
			WriteString(m_TypeWriters[i]->GetReaderName());
		}
		WriteUInt32((uint32_t)m_SharedResourceNames.size());
	}

	void ContentWriter::WriteSharedResources()
	{
		int contador = 0;
		while (m_SharedResources.size() > 0)
		{
			ContentItem* sq = m_SharedResources.front(); m_SharedResources.pop();
			WriteObject(sq);
			contador++;
		}
	}

	void ContentWriter::WriteCompressedOutput()
	{
	}

	void ContentWriter::WriteExternalReference(ExternalReference& reference)
	{
		if (reference.GetFilename().size() == 0)
		{
			WriteString("");
		}
		else
		{
			std::string filename = reference.GetFilename();

			WriteString(filename);
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
			int num = -1;
			//std::map<int, int>::iterator itm = m_SharedResourceNames.find(resource->GetID());
			std::map<ContentItem*, int>::iterator itm = m_SharedResourceNames.find(resource);
			if (itm == m_SharedResourceNames.end())
			{
				num = m_SharedResourceNames.size() + 1;
				m_SharedResourceNames[resource] = num;
				m_SharedResources.push(resource);
			}
			else
				num = (*itm).second;

			WriteInt32(num);
		}
	}
}