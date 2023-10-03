#include "brpch.h"
#include "GPUBuffer.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(GPUBuffer);

	void GPUBuffer::SetMappedData(void* data, size_t dataSize)
	{
		BR_ASSERT(mMappedResource != nullptr && data != nullptr && dataSize > 0 && dataSize <= mDesc.Width);
		memcpy_s(mMappedResource, mDesc.Width, data, dataSize);
	}
}