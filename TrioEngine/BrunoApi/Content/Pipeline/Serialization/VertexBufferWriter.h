#pragma once

#include "TrioApiRequisites.h"

#include "ContentTypeWriter.h"

#include <string>

namespace TrioEngine
{
	class ContentItem;
	class ContentWriter;
	class ContentCompiler;

	class VertexBufferWriter : public ContentTypeWriter
	{
	public:
		VertexBufferWriter();
		~VertexBufferWriter();

		std::string GetWriterName() { m_name = "VertexBufferWriter"; return m_name; }
		std::string GetReaderName() { return "VertexBufferReader"; }

		void Write(ContentWriter* output, ContentItem* value);
	protected:
	};
}