#include "stdafx.h"
#include "MaterialWriter.h"

#include "Content/Pipeline/Graphics/MaterialContent.h"
#include "Content/Pipeline/Serialization/ContentWriter.h"

namespace TrioEngine
{
	MaterialWriter::MaterialWriter()
	{
	}

	MaterialWriter::~MaterialWriter()
	{
	}

	void MaterialWriter::Write(ContentWriter* output, ContentItem* value)
	{
		MaterialContent* inputContent = reinterpret_cast<MaterialContent*>(value);

		output->WriteString(inputContent->GetName());

		output->WriteUInt32(inputContent->GetTextures().size());
		for (auto& pair : inputContent->GetTextures()) {
			output->WriteString(pair.first);
			output->WriteExternalReference(pair.second);
		}
		//TODO: write opaquedata
	}
}