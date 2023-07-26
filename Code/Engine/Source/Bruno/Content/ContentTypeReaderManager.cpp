#include "brpch.h"
#include "ContentTypeReaderManager.h"

#include "Readers/TextureReader.h"

namespace Bruno
{
	std::map<RTTI::IdType, std::shared_ptr<AbstractContentTypeReader>> ContentTypeReaderManager::g_contentTypeReaders;
	bool ContentTypeReaderManager::g_initialized = false;

	const std::map<RTTI::IdType, std::shared_ptr<AbstractContentTypeReader>>& ContentTypeReaderManager::ContentTypeReaders()
	{
		return g_contentTypeReaders;
	}

	bool ContentTypeReaderManager::AddContentTypeReader(std::shared_ptr<AbstractContentTypeReader> reader)
	{
		return g_contentTypeReaders.emplace(reader->GetTargetTypeId(), std::move(reader)).second;
	}

	void ContentTypeReaderManager::Initialize()
	{
		if (g_initialized)
			return;

		AddContentTypeReader(std::make_shared<TextureReader>());

		g_initialized = true;
	}

	void ContentTypeReaderManager::Shutdown()
	{
		g_contentTypeReaders.clear();
		g_initialized = false;
	}
}