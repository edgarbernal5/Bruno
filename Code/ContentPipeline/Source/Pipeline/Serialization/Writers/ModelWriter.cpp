#include "ModelWriter.h"

#include "Pipeline/Graphics/ModelContentItem.h"

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
	}
}
