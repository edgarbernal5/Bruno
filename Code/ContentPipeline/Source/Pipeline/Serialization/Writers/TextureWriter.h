#pragma once

#include "Pipeline/Serialization/ContentTypeWriter.h"

namespace Bruno
{
	class TextureWriter : public AbstractContentTypeWriter
	{
	public:
		TextureWriter();

		virtual std::string GetReaderName() override;
		virtual void Write(ContentWriter& output, const ContentItem& contentItem) override;
	};
}

