#include "stdafx.h"
#include "ImporterManager.h"

#include "TextureImporter.h"
#include "ModelImporter.h"
#include "EffectImporter.h"

namespace TrioEngine
{
	ImporterManager::ImporterManager()
	{
		RegisterStandardTypes();
	}


	ImporterManager::~ImporterManager()
	{
	}

	void ImporterManager::RegisterStandardTypes()
	{
		RegisterTypeImporter<TextureImporter>("TextureImporter");
		RegisterTypeImporter<ModelImporter>("ModelImporter");
		RegisterTypeImporter<EffectImporter>("EffectImporter");
	}

	std::string ImporterManager::GetImporterNameByExtension(std::string const& fileExtension)
	{
		map_type::iterator it = mapImporters.begin();
		std::string name;

		for (; it != mapImporters.end(); ++it)
		{
			IContentImporter* importer = it->second();
			std::size_t index = importer->GetFileExtensions().find(fileExtension);
			if (index != std::string::npos)
			{
				//TO-DO: Verificar que no esté contenida en otra extensión (i.e. bmp en "nbmp")
				name = importer->GetImporterName();
				delete importer;
				break;
			}
			else
			{
				delete importer;
			}
		}
		return name;
	}

	IContentImporter* ImporterManager::GetByImporterName(std::string const& readerName)
	{
		map_type::iterator it = mapImporters.find(readerName);
		if (it != mapImporters.end())
		{
			return it->second();
		}

		return nullptr;
	}
}