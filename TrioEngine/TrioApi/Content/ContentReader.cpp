#include "stdafx.h"
#include "ContentReader.h"

#include "IO/Stream.h"
#include "ContentTypeReaderManager.h"
#include "ContentManager.h"

namespace TrioEngine
{
	ContentReader::ContentReader(ContentManager* contentManager, TrioIO::Stream *stream, std::string assetName) :
		BinaryReader(stream), m_contentManager(contentManager), m_Stream(stream), m_AssetName(assetName)
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

		if (reader.ReadChar() != 'T' || 
			reader.ReadChar() != 'R' || 
			reader.ReadChar() != 'I' || 
			reader.ReadChar() != 'O')
		{
			throw std::exception("bad trio");
		}

		if (reader.ReadByte() != 0) // plataforma windows.
		{
			throw std::exception("bad platform");
		}
		
		int num2 = reader.ReadInt32();
		if (input->CanSeek() && ((num2 - 10) > (input->GetLength() - input->GetPosition())))
		{
			throw std::exception();
		}
		//TO-DO: integrar compresión de archivos.

	}

	void* ContentReader::ReadExternalReference()
	{
		std::string cleanPath = ReadString();
		if (cleanPath.size() == 0)
			return nullptr;

		return m_contentManager->LoadInternal(cleanPath);
	}

	Vector2 ContentReader::ReadVector2()
	{
		Vector2 vector2;
		vector2.x = ReadSingle();
		vector2.y = ReadSingle();
		return vector2;
	}

	Vector3 ContentReader::ReadVector3()
	{
		Vector3 vector2;
		vector2.x = ReadSingle();
		vector2.y = ReadSingle();
		vector2.z = ReadSingle();
		return vector2;
	}

	Vector4 ContentReader::ReadVector4()
	{
		Vector4 vector2;
		vector2.x = ReadSingle();
		vector2.y = ReadSingle();
		vector2.z = ReadSingle();
		vector2.w = ReadSingle();
		return vector2;
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

		m_TypeReaders = ContentTypeReaderManager::ReadTypeManifest(typeCount, this);

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
			num2 = Read(&buffer[0], i, size - i);
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