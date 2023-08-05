#include "brpch.h"
#include "ContentTypeWriterManager.h"

#include "Writers/ShaderWriter.h"
#include "Writers/TextureWriter.h"

namespace Bruno
{
	std::map<RTTI::IdType, std::shared_ptr<AbstractContentTypeWriter>> ContentTypeWriterManager::g_contentTypeWriters;
	bool ContentTypeWriterManager::g_initialized = false;

	const std::map<RTTI::IdType, std::shared_ptr<AbstractContentTypeWriter>>& ContentTypeWriterManager::GetContentTypeWriters()
	{
		return g_contentTypeWriters;
	}

	bool ContentTypeWriterManager::AddContentTypeWriter(std::shared_ptr<AbstractContentTypeWriter> writer)
	{
		return g_contentTypeWriters.emplace(writer->GetTargetTypeId(), std::move(writer)).second;
	}

	void ContentTypeWriterManager::Initialize()
	{
		if (g_initialized)
			return;

		AddContentTypeWriter(std::make_shared<ShaderWriter>());
		AddContentTypeWriter(std::make_shared<TextureWriter>());

		g_initialized = true;
	}

	void ContentTypeWriterManager::Shutdown()
	{
		g_contentTypeWriters.clear();
		g_initialized = false;
	}
}