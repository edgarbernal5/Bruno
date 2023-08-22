#include "ShaderWriter.h"

#include "Pipeline/Graphics/ShaderContentItem.h"
#include <Bruno/Platform/DirectX/D3DCore.h>

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

		for (size_t i = 0; i < Graphics::Core::SHADER_PROGRAMS_COUNT; i++)
		{
			output.WriteBytes(shaderContentItem->CompiledCodeProgramsData[i]);
		}
		
	}
}
