#pragma once

#include "TrioApiRequisites.h"

#include <string>
#include <map>
#include <vector>

namespace TrioEngine
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
			m_TypeReaders2.push_back(new T());
		}
		static std::vector<ContentTypeReader*> ReadTypeManifest(uint32_t typeCount, ContentReader* contentReader);
	private:
		ContentReader* m_Reader;

		static std::map<std::string, ContentTypeReader*> *m_NameToReader;

		static ContentTypeReader* GetTypeReaderByName(std::string name);

		static std::vector<ContentTypeReader*> m_TypeReaders2;


		static ContentTypeReader* GetTypeReader(std::string readerTypeName, ContentReader* contentReader, std::vector<ContentTypeReader*> &newTypeReaders);
		static void AddTypeReader(std::string readerTypeName, ContentReader* contentReader, ContentTypeReader* reader);

	};
}