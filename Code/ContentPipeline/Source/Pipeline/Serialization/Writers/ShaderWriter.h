#pragma once

#include "Pipeline/Serialization/ContentTypeWriter.h"

namespace Bruno
{
	class ShaderWriter : public AbstractContentTypeWriter
	{
	public:
		ShaderWriter();

		virtual void Write(ContentWriter& output, const ContentItem& contentItem) override;
	};
}

