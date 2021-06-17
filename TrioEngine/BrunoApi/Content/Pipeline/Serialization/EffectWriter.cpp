#include "stdafx.h"
#include "EffectWriter.h"

#include "ContentWriter.h"
#include "Content/Pipeline/Graphics/CompiledEffectContent.h"

namespace TrioEngine
{
	EffectWriter::EffectWriter()
	{
	}

	EffectWriter::~EffectWriter()
	{
	}

	void EffectWriter::Write(ContentWriter *output, ContentItem* value)
	{
		CompiledEffectContent* input = dynamic_cast<CompiledEffectContent*>(value);

		output->WriteUInt32(input->GetEffectCode().size());
		output->WriteBytes(input->GetEffectCode().data(), input->GetEffectCode().size());
	}
}