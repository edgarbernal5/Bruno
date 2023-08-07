#pragma once

#include "Pipeline/Serialization/ContentTypeWriter.h"

namespace Bruno
{
	class MaterialWriter : public AbstractContentTypeWriter
	{
	public:
		MaterialWriter();

		virtual std::string GetReaderName() override;
		virtual void Write(ContentWriter& output, const ContentItem& contentItem) override;
	};
}

