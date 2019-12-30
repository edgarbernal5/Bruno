#include "stdafx.h"
#include "ExternalReferenceWriter.h"

#include "ContentWriter.h"
//#include "Content/Pipeline/Graphics/CompiledEffectContent.h"

namespace TrioEngine
{
	ExternalReferenceWriter::ExternalReferenceWriter()
	{
	}

	ExternalReferenceWriter::~ExternalReferenceWriter()
	{
	}

	void ExternalReferenceWriter::Write(ContentWriter *output, ContentItem* value)
	{
		output->WriteExternalReference(*((ExternalReference*)value));
	}
}