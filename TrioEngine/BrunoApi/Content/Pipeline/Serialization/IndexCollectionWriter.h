#pragma once

#include "TrioApiRequisites.h"

#include "ContentTypeWriter.h"
#include <string>

namespace TrioEngine
{
	class IndexCollectionWriter : public ContentTypeWriter
	{
	public:
		IndexCollectionWriter();
		~IndexCollectionWriter();

		std::string GetWriterName() { m_name = "IndexCollectionWriter"; return m_name; }
		std::string GetReaderName() { return "IndexCollectionReader"; }
		void Write(ContentWriter *output, ContentItem* value);
	};
}