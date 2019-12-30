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

		std::string GetWriterName() { m_Name = "IndexCollectionWriter"; return m_Name; }
		std::string GetReaderName() { return "IndexCollectionReader"; }
		void Write(ContentWriter *output, ContentItem* value);
	};
}