#pragma once

#include "D3DCommon.h"
#include "Resource.h"
#include "Resources.h"

namespace Bruno
{
	class GPUBuffer;

	struct BufferUpload
	{
		GPUBuffer* mBuffer = nullptr;
		std::unique_ptr<uint8_t[]> mBufferData;
		size_t mBufferDataSize = 0;
	};

	class GPUBuffer : public Resource
	{
		BR_RTTI_DECLARATION(GPUBuffer, Resource);

	public:
		void SetMappedData(void* data, size_t dataSize);

	protected:
		uint8_t* mMappedResource = nullptr;
		uint32_t mStride = 0;
		DescriptorHandle mCBVDescriptor{};
		DescriptorHandle mSRVDescriptor{};
		DescriptorHandle mUAVDescriptor{};
	};
}

