#pragma once

#include "Pipeline/Serialization/ContentTypeWriter.h"

namespace Bruno
{
	class ShaderWriter : public AbstractContentTypeWriter
	{
	public:
		ShaderWriter();

		virtual std::string GetReaderName() override;
		virtual void Write(ContentWriter& output, const ContentItem& contentItem) override;
	};
}

