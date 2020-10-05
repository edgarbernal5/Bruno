#include "stdafx.h"
#include "ContentReader.h"

#include "IO/Stream.h"
#include "IO/Path.h"

#include "ContentTypeReaderManager.h"
#include "ContentManager.h"

#include "Errors/ContentPipelineException.h"

namespace TrioEngine
{
	ContentReader::ContentReader(ContentManager* contentManager, TrioIO::Stream *stream, std::string assetName) :
		BinaryReader(stream), m_contentManager(contentManager), m_stream(stream), m_assetName(assetName)
	{
	}

	ContentReader::~ContentReader()
	{
	}

	ContentReader* ContentReader::Create(ContentManager* contentManager, TrioIO::Stream *stream, std::string assetName)
	{
		PrepareStream(stream);
		ContentReader* reader = new ContentReader(contentManager, stream, assetName);
		return reader;
	}

	ContentManager* ContentReader::GetContentManager()
	{
		return m_contentManager;
	}

	void ContentReader::PrepareStream(TrioIO::Stream* input)
	{
		BinaryReader reader(input);

		if (reader.ReadChar() != 'E' || 
			reader.ReadChar() != 'S' || 
			reader.ReadChar() != 'T' || 
			reader.ReadChar() != 'E' ||
			reader.ReadChar() != 'R' ||
			reader.ReadChar() != 'O')
		{
			throw ContentPipelineException("bad file header");
		}

		if (reader.ReadByte() != 0) // plataforma windows.
		{
			throw ContentPipelineException("bad platform");
		}
		
		int totalSizeInBytes = reader.ReadInt32();
		int headerAndContentSize = HeaderSize + sizeof(int);
		if (input->CanSeek() && ((totalSizeInBytes - headerAndContentSize) > (input->GetLength() - input->GetPosition())))
		{
			throw ContentPipelineException("corrupt file");
		}
		//TO-DO: integrar compresión de archivos.

	}

	void* ContentReader::ReadExternalReference()
	{
		std::string referenceName = ReadString();
		if (referenceName.size() == 0)
			return nullptr;

		std::size_t directorySeparatorIndex = m_assetName.find_last_of("/\\");
		std::string path;

		if (directorySeparatorIndex != std::string::npos) {
			path = m_assetName.substr(0, directorySeparatorIndex);
		}
		std::string cleandPath = TrioIO::Path::Combine(path, referenceName);

		return m_contentManager->LoadInternal(cleandPath);
	}

	Vector2 ContentReader::ReadVector2()
	{
		Vector2 vector;
		vector.x = ReadSingle();
		vector.y = ReadSingle();
		return vector;
	}

	Vector3 ContentReader::ReadVector3()
	{
		Vector3 vector;
		vector.x = ReadSingle();
		vector.y = ReadSingle();
		vector.z = ReadSingle();
		return vector;
	}

	Vector4 ContentReader::ReadVector4()
	{
		Vector4 vector;
		vector.x = ReadSingle();
		vector.y = ReadSingle();
		vector.z = ReadSingle();
		vector.w = ReadSingle();
		return vector;
	}

	Matrix ContentReader::ReadMatrix()
	{
		Matrix matrix;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
				matrix.m[i][j] = ReadSingle();
		}
		return matrix;
	}

	uint32_t ContentReader::ReadHeader()
	{
		uint32_t typeCount = ReadUInt32();

		m_typeReaders = ContentTypeReaderManager::ReadTypeManifest(typeCount, this);

		uint32_t sharedResources = ReadUInt32();

		if (sharedResources > 0)
		{
			m_sharedResourceFixups = std::vector< std::vector<std::function<void(ContentItem*)>> >(sharedResources);
		}

		return sharedResources;
	}
	
	std::vector<uint8_t> ContentReader::ReadByteBuffer(int size)
	{
		int num2 = 0;
		std::vector<uint8_t> buffer(size);
		for (int i = 0; i < size; i += num2)
		{
			num2 = Read(buffer.data(), i, size - i);
			if (num2 == 0)
			{
				throw std::exception("bad xnb");
			}
		}
		return buffer;
	}

	void ContentReader::ReadSharedResource(std::function<void(ContentItem*)> action)
	{
		int index = ReadInt32();
		if (index != 0)
		{
			index--;
			if (index >= m_sharedResourceFixups.size())
			{
				throw std::exception();
			}
			m_sharedResourceFixups[index].push_back(action);
		}
	}

	void ContentReader::ReadSharedResources(uint32_t sharedResourceCount)
	{
		if (sharedResourceCount > 0)
		{
			std::vector<ContentItem*> objArray(sharedResourceCount, nullptr);
			for (int i = 0; i < sharedResourceCount; i++)
			{
				objArray[i] = ReadObject<ContentItem>();
			}

			for (int i = 0; i < sharedResourceCount; i++)
			{
				for (int j = 0; j < m_sharedResourceFixups[i].size(); j++)
				{
					m_sharedResourceFixups[i][j](objArray[i]);
				}
			}
		}
	}
}