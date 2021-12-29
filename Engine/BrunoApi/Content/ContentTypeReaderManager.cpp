#include "stdafx.h"
#include "ContentTypeReaderManager.h"

#include "ContentTypeReader.h"
#include "ContentReader.h"

#include "Pipeline/Readers/ExternalReferenceReader.h"
#include "Pipeline/Readers/ModelReader.h"
#include "Pipeline/Readers/Texture2DReader.h"
#include "Pipeline/Readers/VertexBufferReader.h"
#include "Pipeline/Readers/IndexCollectionReader.h"
#include "Pipeline/Readers/MaterialReader.h"
#include "Pipeline/Readers/EffectReader.h"

namespace BrunoEngine
{
	std::map<std::string, ContentTypeReader*>* ContentTypeReaderManager::m_nameToReader = new std::map<std::string, ContentTypeReader*>();
	std::vector<ContentTypeReader*> ContentTypeReaderManager::m_typeReaders2;

	ContentTypeReaderManager::ContentTypeReaderManager(ContentReader* reader)
		: m_reader(reader)
	{
	}

	ContentTypeReaderManager::~ContentTypeReaderManager()
	{
	}

	ContentTypeReader* ContentTypeReaderManager::GetTypeReaderByName(std::string name)
	{
		for (int i = 0; i < m_typeReaders2.size(); i++)
		{
			if (m_typeReaders2[i]->GetReaderName() == name)
				return m_typeReaders2[i];
		}
		return nullptr;
	}

	void ContentTypeReaderManager::RegisterStandardTypes()
	{
		RegisterContentTypeReader<Texture2DReader>();
		RegisterContentTypeReader<ModelReader>();
		RegisterContentTypeReader<VertexBufferReader>();
		RegisterContentTypeReader<IndexCollectionReader>();
		RegisterContentTypeReader<MaterialReader>();
		RegisterContentTypeReader<EffectReader>();
		//RegisterContentTypeReader<EffectMaterialReader>();
		RegisterContentTypeReader<ExternalReferenceReader>();
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

		auto itm = m_nameToReader->find(readerTypeName);
		if (itm != m_nameToReader->end())
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