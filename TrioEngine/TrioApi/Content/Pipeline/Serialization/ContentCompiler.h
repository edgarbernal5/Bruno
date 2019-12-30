#pragma once

#include "TrioApiRequisites.h"

#include <map>
#include <string>
#include <vector>

namespace TrioIO
{
	class Stream;
}

namespace TrioEngine
{
	class ContentTypeWriter;
	class ContentItem;
	class ContentTypeWriterFactory;

	class ContentCompiler
	{
	public:
		ContentCompiler();
		~ContentCompiler();

		void Compile(TrioIO::Stream *stream, ContentItem *object, bool compressContent);

		ContentTypeWriter* GetTypeWriter(std::string name, std::vector<std::string> &dependencies);
	private:
		std::map<std::string, ContentTypeWriter*> m_WriterInstances;

		std::map<ContentTypeWriter*, std::vector<std::string> > m_WriterDependencies;

		ContentTypeWriter* GetTypeWriterInternal(std::string name);

		ContentTypeWriterFactory *m_Factory;

		void AddTypeWriter(ContentTypeWriter* writer);
	};
}