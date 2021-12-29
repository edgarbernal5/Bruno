#include "stdafx.h"
#include "IndexCollectionReader.h"

#include "Content/GraphicsContentHelper.h"
#include "Content/ContentReader.h"

#include "Graphics/IndexBuffer.h"

namespace BrunoEngine
{
	IndexCollectionReader::IndexCollectionReader()
	{
	}


	IndexCollectionReader::~IndexCollectionReader()
	{
	}

	void* IndexCollectionReader::Read(ContentReader* input)
	{
		int indexCount;
		IndexElementSize sixteenBits;

		bool flag = input->ReadBoolean();
		int bufferSize = input->ReadInt32();

		std::vector<uint8_t> dataInBytes = input->ReadByteBuffer(bufferSize);
		if (flag)
		{
			sixteenBits = IndexElementSize::SixteenBits;
			indexCount = bufferSize / 2;
		}
		else
		{
			sixteenBits = IndexElementSize::ThirtyTwoBits;
			indexCount = bufferSize / 4;
		}

		IndexBuffer* ib = new IndexBuffer(GraphicsContentHelper::GraphicsDeviceFromContentReader(input), sixteenBits, indexCount);
		ib->SetData<uint8_t>(0, dataInBytes.data(), dataInBytes.size(), 0, indexCount, SetDataOptions::None);

		return ib;
	}
}