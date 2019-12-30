#include "stdafx.h"
#include "ExternalReferenceReader.h"

#include "ContentManager.h"
#include "ContentReader.h"
#include "Content/Pipeline/ContentItem.h"

namespace TrioEngine
{
	ExternalReferenceReader::ExternalReferenceReader()
	{
	}

	ExternalReferenceReader::~ExternalReferenceReader()
	{
	}

	void* ExternalReferenceReader::Read(ContentReader* input)
	{
		return input->ReadExternalReference();
	}
}