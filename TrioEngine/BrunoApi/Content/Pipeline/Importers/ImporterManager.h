#pragma once

#include "TrioApiRequisites.h"

#include <map>
#include <string>

namespace BrunoEngine
{
	class IContentImporter;

	class BRUNO_API_EXPORT ImporterManager
	{
	public:
		ImporterManager();
		~ImporterManager();

		IContentImporter* GetByImporterName(std::string const& readerName);
		std::string GetImporterNameByExtension(std::string const& fileExtension);
		void RegisterStandardTypes();

	private:
		template<typename T>
		static IContentImporter* CreateInstance();

		typedef std::map<std::string, IContentImporter*(*)()> map_type;

		map_type mapImporters;

		template<typename T>
		void RegisterTypeImporter(std::string nameImporter);
	};

	template<typename T>
	IContentImporter* ImporterManager::CreateInstance() { return new T(); }

	template<typename T>
	void ImporterManager::RegisterTypeImporter(std::string nameImporter)
	{
		mapImporters[nameImporter] = &CreateInstance <T>;
	}
}