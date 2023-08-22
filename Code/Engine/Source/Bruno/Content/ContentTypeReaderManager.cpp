#include "brpch.h"
#include "ContentTypeReaderManager.h"

#include "Readers/TextureReader.h"
#include "Readers/ShaderReader.h"
#include "Readers/ModelReader.h"
#include "Readers/MaterialReader.h"
#include "Readers/ExternalReferenceReader.h"

namespace Bruno
{
	std::map<RTTI::IdType, std::shared_ptr<AbstractContentTypeReader>> ContentTypeReaderManager::g_contentTypeReaders;
	bool ContentTypeReaderManager::g_initialized = false;

	const std::map<RTTI::IdType, std::shared_ptr<AbstractContentTypeReader>>& ContentTypeReaderManager::GetContentTypeReaders()
	{
		return g_contentTypeReaders;
	}

	bool ContentTypeReaderManager::AddContentTypeReader(std::shared_ptr<AbstractContentTypeReader> reader)
	{
		return g_contentTypeReaders.emplace(reader->GetTargetTypeId(), std::move(reader)).second;
	}


	std::shared_ptr<AbstractContentTypeReader> ContentTypeReaderManager::GetReaderByName(const std::string& name)
	{
		for (auto& [rtti, reader] : g_contentTypeReaders)
		{
			if (reader->ToString() == name)
				return reader;
		}
		return nullptr;
	}

	void ContentTypeReaderManager::Initialize()
	{
		if (g_initialized)
			return;

		AddContentTypeReader(std::make_shared<TextureReader>());
		AddContentTypeReader(std::make_shared<ShaderReader>());
		AddContentTypeReader(std::make_shared<ModelReader>());
		AddContentTypeReader(std::make_shared<MaterialReader>());
		AddContentTypeReader(std::make_shared<ExternalReferenceReader>());

		g_initialized = true;
	}

	void ContentTypeReaderManager::Shutdown()
	{
		g_contentTypeReaders.clear();
		g_initialized = false;
	}
}