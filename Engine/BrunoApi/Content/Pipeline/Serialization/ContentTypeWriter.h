#pragma once

#include "TrioApiRequisites.h"

#include <string>

namespace BrunoEngine
{
	class ContentItem;
	class ContentWriter;
	class ContentCompiler;

	class ContentTypeWriter
	{
	public:
		ContentTypeWriter() {}
		virtual ~ContentTypeWriter(){}

		virtual std::string GetWriterName() = 0;
		virtual std::string GetReaderName() = 0;

		virtual void Initialize(ContentCompiler *compiler) {}
		virtual void Write(ContentWriter *output, ContentItem* value) = 0;

		bool operator <(const ContentTypeWriter &other) const
		{
			return m_name < other.m_name;
		}
		bool operator <(const ContentTypeWriter *other) const
		{
			return m_name < other->m_name;
		}
	protected:
		std::string m_name;
	};
}