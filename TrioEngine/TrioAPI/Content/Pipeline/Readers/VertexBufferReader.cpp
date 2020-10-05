#include "stdafx.h"
#include "VertexBufferReader.h"

#include "Content/GraphicsContentHelper.h"
#include "Content/ContentReader.h"

#include "Graphics/VertexBuffer.h"

namespace TrioEngine
{
	VertexBufferReader::VertexBufferReader()
	{
	}


	VertexBufferReader::~VertexBufferReader()
	{
	}

	void* VertexBufferReader::Read(ContentReader* input)
	{
		int vertexStride = input->ReadInt32();
		int vertexElementsSize = input->ReadInt32();
		std::vector<VertexElement> elements(vertexElementsSize);
		for (int i = 0; i < vertexElementsSize; i++)
		{
			VertexElement element;
			element.SetOffset(input->ReadInt32());
			element.SetUsageIndex(input->ReadInt32());
			element.SetVertexElementFormat((VertexElementFormat)input->ReadInt32());
			element.SetVertexElementUsage((VertexElementUsage)input->ReadInt32());

			elements[i] = element;
		}

		VertexDeclaration* vertexDeclaration = new VertexDeclaration(vertexStride, elements);

		uint32_t vertexCount = input->ReadUInt32();
		int size = vertexCount * vertexStride;
		std::vector<uint8_t> dataInBytes = input->ReadByteBuffer(size);

		VertexBuffer* buffer = new VertexBuffer(GraphicsContentHelper::GraphicsDeviceFromContentReader(input), vertexDeclaration, vertexCount);
		//buffer->SetData<uint8_t>(&data[0], data.size());
		buffer->SetData<uint8_t>(0, dataInBytes.data(), dataInBytes.size(), 0, vertexCount, vertexStride, SetDataOptions::None);
		return buffer;
	}
}