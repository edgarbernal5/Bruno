#pragma once

#include "Pipeline/Serialization/ContentTypeWriter.h"

namespace Bruno
{
	class ExternalReferenceWriter : public AbstractContentTypeWriter
	{
	public:
		ExternalReferenceWriter();

		virtual std::string GetReaderName() override;
		virtual void Write(ContentWriter& output, const ContentItem& contentItem) override;
	};
}

