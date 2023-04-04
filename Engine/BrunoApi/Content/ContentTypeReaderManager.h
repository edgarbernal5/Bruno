#pragma once

#include "TrioApiRequisites.h"

#include <string>
#include <map>
#include <vector>

namespace BrunoEngine
{
	class ContentReader;
	class ContentTypeReader;

	class ContentTypeReaderManager
	{
	public:
		ContentTypeReaderManager(ContentReader* reader);
		~ContentTypeReaderManager();

		static void RegisterStandardTypes();

		template<typename T> static void RegisterContentTypeReader()
		{
			m_typeReaders.push_back(new T());
		}

		static std::vector<ContentTypeReader*> ReadTypeManifest(uint32_t typeCount, ContentReader* contentReader);
	private:
		ContentReader* m_reader;

		static std::map<std::string, ContentTypeReader*> *m_nameToReader;

		static ContentTypeReader* GetTypeReaderByName(std::string name);

		static std::vector<ContentTypeReader*> m_typeReaders;

		static ContentTypeReader* GetTypeReader(std::string readerTypeName, ContentReader* contentReader, std::vector<ContentTypeReader*> &newTypeReaders);
		static void AddTypeReader(std::string readerTypeName, ContentReader* contentReader, ContentTypeReader* reader);

	};
}