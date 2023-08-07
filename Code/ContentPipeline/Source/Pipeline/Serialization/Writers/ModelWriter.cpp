#include "ModelWriter.h"

#include "Pipeline/Graphics/ModelContentItem.h"
#include "Pipeline/Graphics/MaterialContentItem.h"

namespace Bruno
{
	ModelWriter::ModelWriter() :
		AbstractContentTypeWriter(ModelContentItem::GetTypeIdClass())
	{
	}

	std::string ModelWriter::GetReaderName()
	{
		return "ModelReader";
	}

	void ModelWriter::Write(ContentWriter& output, const ContentItem& contentItem)
	{
		auto modelContentItem = contentItem.As<ModelContentItem>();
		output.WriteUInt64(modelContentItem->Materials.size());
		for (size_t i = 0; i < modelContentItem->Materials.size(); i++)
		{
			output.WriteString(modelContentItem->Materials[i]->Name);
			output.WriteUInt64(modelContentItem->Materials[i]->m_textureByKeys.size());
			for (auto& [name, reference] : modelContentItem->Materials[i]->m_textureByKeys)
			{
				output.WriteString(name);
				output.WriteExternalReference(reference);
			}
		}
	}
}
