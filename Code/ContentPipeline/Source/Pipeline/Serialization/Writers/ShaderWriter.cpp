#include "ShaderWriter.h"

#include "Pipeline/Graphics/ShaderContentItem.h"

namespace Bruno
{
	ShaderWriter::ShaderWriter() : 
		AbstractContentTypeWriter(ShaderContentItem::GetTypeIdClass())
	{
	}

	void ShaderWriter::Write(ContentWriter& output, const ContentItem& contentItem)
	{
		auto shaderContentItem = contentItem.As<ShaderContentItem>();
		output.WriteUInt32(shaderContentItem->CompiledCodeData.size());
		output.WriteBytes(shaderContentItem->CompiledCodeData.data(), shaderContentItem->CompiledCodeData.size());
	}
}
