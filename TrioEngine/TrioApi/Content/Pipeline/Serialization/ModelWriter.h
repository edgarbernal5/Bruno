#pragma once

#include "TrioApiRequisites.h"

#include "ContentTypeWriter.h"
#include <string>

namespace TrioEngine
{
	class ModelWriter : public ContentTypeWriter
	{
	public:
		ModelWriter();
		~ModelWriter();

		std::string GetWriterName() { m_Name = "ModelWriter"; return m_Name; }
		std::string GetReaderName() { return "ModelReader"; }
		void Write(ContentWriter *output, ContentItem* value);
	};
}