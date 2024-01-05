#include "brpch.h"
#include "ExternalReferenceReader.h"

#include "Bruno/Content/ContentReader.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(ExternalReferenceReader);

	ExternalReferenceReader::ExternalReferenceReader() :
		AbstractContentTypeReader(0)
	{
	}

	std::shared_ptr<Asset> ExternalReferenceReader::Read(ContentReader& input)
	{
		return input.ReadExternalReference();
	}

}