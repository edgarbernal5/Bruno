#pragma once

#include "TrioApiRequisites.h"

#include "ContentTypeWriter.h"
#include <string>

namespace TrioEngine
{
	class EffectWriter : public ContentTypeWriter
	{
	public:
		EffectWriter();
		~EffectWriter();

		std::string GetWriterName() { m_name = "EffectWriter"; return m_name; }
		std::string GetReaderName() { return "EffectReader"; }
		void Write(ContentWriter *output, ContentItem* value);
	};
}