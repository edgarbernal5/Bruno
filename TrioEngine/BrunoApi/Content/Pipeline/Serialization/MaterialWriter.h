#pragma once

#include "TrioApiRequisites.h"

#include "ContentTypeWriter.h"
#include <string>

namespace BrunoEngine
{
	class MaterialWriter : public ContentTypeWriter
	{
	public:
		MaterialWriter();
		~MaterialWriter();

		std::string GetWriterName() { m_name = "MaterialWriter"; return m_name; }
		std::string GetReaderName() { return "MaterialReader"; }
		void Write(ContentWriter *output, ContentItem* value);
	};
}