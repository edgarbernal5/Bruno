#include "stdafx.h"
#include "VertexBufferWriter.h"

#include "ContentWriter.h"
#include "Content/Pipeline/Graphics/VertexDeclarationContent.h"
#include "Content/Pipeline/Graphics/VertexBufferContent.h"

namespace TrioEngine
{
	VertexBufferWriter::VertexBufferWriter()
	{
	}


	VertexBufferWriter::~VertexBufferWriter()
	{
	}

	void VertexBufferWriter::Write(ContentWriter* output, ContentItem* value)
	{
		VertexBufferContent* content = (VertexBufferContent*)value;

		output->WriteInt32(*content->GetVertexDeclaration()->GetVertexStride());
		output->WriteUInt32(content->GetVertexDeclaration()->GetVertexElements().size());

		for (int i = 0; i < content->GetVertexDeclaration()->GetVertexElements().size(); i++)
		{
			VertexElement element = content->GetVertexDeclaration()->GetVertexElements()[i];
			output->WriteInt32(element.GetOffset());
			output->WriteInt32(element.GetUsageIndex());
			output->WriteInt32((int)element.GetVertexElementFormat());
			output->WriteInt32((int)element.GetVertexElementUsage());
		}

		output->WriteUInt32((uint32_t)(content->GetVertexDataSize() / *content->GetVertexDeclaration()->GetVertexStride()));
		output->WriteBytes(content->GetVertexData(), content->GetVertexDataSize());
	}
}