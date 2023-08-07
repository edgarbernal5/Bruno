#include "brpch.h"
#include "ContentTypeWriterManager.h"

#include "Writers/ShaderWriter.h"
#include "Writers/TextureWriter.h"
#include "Writers/ModelWriter.h"
#include "Writers/MaterialWriter.h"
#include "Writers/ExternalReferenceWriter.h"

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
		AddContentTypeWriter(std::make_shared<ModelWriter>());
		AddContentTypeWriter(std::make_shared<MaterialWriter>());
		AddContentTypeWriter(std::make_shared<ExternalReferenceWriter>());

		g_initialized = true;
	}

	void ContentTypeWriterManager::Shutdown()
	{
		g_contentTypeWriters.clear();
		g_initialized = false;
	}
}