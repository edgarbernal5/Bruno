#pragma once

#include "Pipeline/Serialization/ContentTypeWriter.h"

namespace Bruno
{
	class ModelWriter : public AbstractContentTypeWriter
	{
	public:
		ModelWriter();

		virtual std::string GetReaderName() override;
		virtual void Write(ContentWriter& output, const ContentItem& contentItem) override;
	};
}

