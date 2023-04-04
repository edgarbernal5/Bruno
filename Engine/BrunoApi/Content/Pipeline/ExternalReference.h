#pragma once

#include "TrioApiRequisites.h"
#include "ContentItem.h"
#include "IO/Path.h"

namespace BrunoEngine
{
	class ExternalReference : public ContentItem
	{
	public:
		ExternalReference() : ContentItem(), m_filename("")
		{
			m_processorName = "ExternalReferenceWriter";
		}

		ExternalReference(std::string filename) : ContentItem()
		{
			if (filename.size() > 0)
			{
				m_filename = BrunoIO::Path::GetFullDirectory(filename);
			}
			m_processorName = "ExternalReferenceWriter";
		}

		inline std::string GetFilename()
		{
			return m_filename;
		}

		inline void SetFilename(std::string filename)
		{
			m_filename = filename;
		}
	private:
		std::string m_filename;
	};
}