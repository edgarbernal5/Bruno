#include "ShaderWriter.h"

#include "Pipeline/Graphics/ShaderContentItem.h"

namespace Bruno
{
	ShaderWriter::ShaderWriter() : 
		AbstractContentTypeWriter(ShaderContentItem::GetTypeIdClass())
	{
	}

	std::string ShaderWriter::GetReaderName()
	{
		return "ShaderReader";
	}

	void ShaderWriter::Write(ContentWriter& output, const ContentItem& contentItem)
	{
		auto shaderContentItem = contentItem.As<ShaderContentItem>();
		output.WriteBytes(shaderContentItem->CompiledCodeData);
	}
}
