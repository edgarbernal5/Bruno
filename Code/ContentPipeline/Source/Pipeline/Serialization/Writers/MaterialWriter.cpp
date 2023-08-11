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
		auto materialContentItem = contentItem.As<MaterialContentItem>();

		output.WriteString(materialContentItem->Name);
		output.WriteUInt64(materialContentItem->TexturesByKeys.size());
		for (auto& [name, reference] : materialContentItem->TexturesByKeys)
		{
			output.WriteString(name);
			output.WriteExternalReference(reference);
		}
	}
}
