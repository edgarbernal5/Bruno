#pragma once

#include "TrioApiRequisites.h"


#include "Graphics/SurfaceFormat.h"
#include "ContentTypeWriter.h"
#include <string>

namespace TrioEngine
{
	class Texture2DContent;

	class Texture2DWriter : public ContentTypeWriter
	{
	public:
		Texture2DWriter();
		~Texture2DWriter();

		std::string GetWriterName() { m_Name = "Texture2DWriter"; return m_Name; }
		std::string GetReaderName() { return "Texture2DReader"; }
		void Write(ContentWriter *output, ContentItem* value);

	private:
		void WriteTextureHeader(ContentWriter* output, SurfaceFormat format, int width, int height, int depth, int mipLevels);
		void WriteTextureData(ContentWriter* output, Texture2DContent* texture);
	};
}