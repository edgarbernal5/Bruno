#include "stdafx.h"
#include "ExternalReferenceReader.h"

#include "Content/ContentReader.h"
#include "Content/Pipeline/ContentItem.h"

namespace BrunoEngine
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