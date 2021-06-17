#pragma once

#include "TrioApiRequisites.h"

#include <map>
#include <string>
#include <vector>

namespace TrioIO
{
	class Stream;
}

namespace BrunoEngine
{
	class ContentTypeWriter;
	class ContentItem;
	class ContentTypeWriterFactory;

	class ContentCompiler
	{
	public:
		ContentCompiler();
		~ContentCompiler();

		void Compile(TrioIO::Stream *stream, ContentItem *object, bool compressContent, std::string rootDirectory, std::string referenceRelocationPath);

		ContentTypeWriter* GetTypeWriter(std::string name, std::vector<std::string> &dependencies);
	private:
		std::map<std::string, ContentTypeWriter*> m_writerInstances;
		std::map<ContentTypeWriter*, std::vector<std::string> > m_writerDependencies;
		ContentTypeWriterFactory *m_factory;

		void AddTypeWriter(ContentTypeWriter* writer);
		ContentTypeWriter* GetTypeWriterInternal(std::string name);
	};
}