#pragma once

#include "ContentTypeWriter.h"
#include <Bruno/Core/RTTI.h>

#include <map>

namespace Bruno
{
	class ContentTypeWriterManager
	{
	public:
		ContentTypeWriterManager() = delete;
		~ContentTypeWriterManager() = default;

		static const std::map<RTTI::IdType, std::shared_ptr<AbstractContentTypeWriter>>& GetContentTypeWriters();
		static bool AddContentTypeWriter(std::shared_ptr<AbstractContentTypeWriter> reader);

		static void Initialize();
		static void Shutdown();

	private:
		static std::map<RTTI::IdType, std::shared_ptr<AbstractContentTypeWriter>> g_contentTypeWriters;
		static bool g_initialized;
	};
}