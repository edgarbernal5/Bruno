#include "ExternalReferenceWriter.h"

#include "Pipeline/ExternalReferenceContentItem.h"

namespace Bruno
{
	ExternalReferenceWriter::ExternalReferenceWriter() :
		AbstractContentTypeWriter(ExternalReferenceContentItem::GetTypeIdClass())
	{
	}

	std::string ExternalReferenceWriter::GetReaderName()
	{
		return "ExternalReferenceReader";
	}

	void ExternalReferenceWriter::Write(ContentWriter& output, const ContentItem& contentItem)
	{
		auto& reference = *reinterpret_cast<ExternalReferenceContentItem*>(const_cast<ContentItem*>(&contentItem));
		output.WriteExternalReference(reference);
	}
}
