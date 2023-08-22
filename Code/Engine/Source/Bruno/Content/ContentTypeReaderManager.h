#pragma once

#include "ContentTypeReader.h"
#include <Bruno/Core/RTTI.h>

#include <map>

namespace Bruno
{
	class ContentTypeReaderManager
	{
	public:
		ContentTypeReaderManager() = delete;
		~ContentTypeReaderManager() = default;

		static const std::map<RTTI::IdType, std::shared_ptr<AbstractContentTypeReader>>& GetContentTypeReaders();
		static std::shared_ptr<AbstractContentTypeReader> GetReaderByName(const std::string& name);
		static bool AddContentTypeReader(std::shared_ptr<AbstractContentTypeReader> reader);

		static void Initialize();
		static void Shutdown();

	private:
		static std::map<RTTI::IdType, std::shared_ptr<AbstractContentTypeReader>> g_contentTypeReaders;
		static bool g_initialized;
	};
}