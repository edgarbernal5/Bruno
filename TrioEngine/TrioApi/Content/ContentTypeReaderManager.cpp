#include "stdafx.h"
#include "ContentTypeReaderManager.h"

#include "ContentTypeReader.h"
#include "ContentReader.h"

namespace TrioEngine
{
	std::map<std::string, ContentTypeReader*>* ContentTypeReaderManager::m_NameToReader = new std::map<std::string, ContentTypeReader*>();
	std::vector<ContentTypeReader*> ContentTypeReaderManager::m_TypeReaders2;

	ContentTypeReaderManager::ContentTypeReaderManager(ContentReader* reader)
		: m_Reader(reader)
	{
	}

	ContentTypeReaderManager::~ContentTypeReaderManager()
	{
	}

	ContentTypeReader* ContentTypeReaderManager::GetTypeReaderByName(std::string name)
	{
		for (int i = 0; i < m_TypeReaders2.size(); i++)
		{
			if (m_TypeReaders2[i]->GetReaderName() == name)
				return m_TypeReaders2[i];
		}
		return nullptr;
	}

	void ContentTypeReaderManager::RegisterStandardTypes()
	{
		//RegisterContentTypeReader<Texture2DReader>();
		//RegisterContentTypeReader<ModelReader>();
		//RegisterContentTypeReader<VertexBufferReader>();
		//RegisterContentTypeReader<IndexCollectionReader>();
		//RegisterContentTypeReader<MaterialReader>();
		//RegisterContentTypeReader<EffectReader>();
		//RegisterContentTypeReader<EffectMaterialReader>();
		//RegisterContentTypeReader<ExternalReferenceReader>();
	}

	std::vector<ContentTypeReader*> ContentTypeReaderManager::ReadTypeManifest(uint32_t typeCount, ContentReader* contentReader)
	{
		std::vector<ContentTypeReader*> readers(typeCount);

		std::vector<ContentTypeReader*> newTypeReaders;
		for (int i = 0; i < typeCount; i++)
		{
			ContentTypeReader* reader = GetTypeReader(contentReader->ReadString(), contentReader, newTypeReaders);
			//TO-DO: Verificacion de versiones:
			readers[i] = reader;

		}

		if (newTypeReaders.size() > 0)
		{
		}
		return readers;
	}

	ContentTypeReader* ContentTypeReaderManager::GetTypeReader(std::string readerTypeName, ContentReader* contentReader, std::vector<ContentTypeReader*> &newTypeReaders)
	{
		ContentTypeReader* reader = nullptr;

		std::map<std::string, ContentTypeReader*>::iterator itm = m_NameToReader->find(readerTypeName);
		if (itm != m_NameToReader->end())
			return (*itm).second;

		reader = GetTypeReaderByName(readerTypeName);
		AddTypeReader(readerTypeName, contentReader, reader);
		newTypeReaders.push_back(reader);
		return reader;
	}

	void ContentTypeReaderManager::AddTypeReader(std::string readerTypeName, ContentReader* contentReader, ContentTypeReader* reader)
	{
	}
}