#include "MaterialWriter.h"

#include "Pipeline/Graphics/MaterialContentItem.h"

namespace Bruno
{
	MaterialWriter::MaterialWriter() :
		AbstractContentTypeWriter(MaterialContentItem::GetTypeIdClass())
	{
	}

	std::string MaterialWriter::GetReaderName()
	{
		return "MaterialReader";
	}

	void MaterialWriter::Write(ContentWriter& output, const ContentItem& contentItem)
	{
		
	}
}
