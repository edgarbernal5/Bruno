#pragma once

#include "D3DCommon.h"
#include "Resource.h"
#include "Resources.h"

namespace Bruno
{
	class GPUBuffer;

	enum class BufferAccessFlags : uint8_t
	{
		gpuOnly = 0,
		hostWritable = 1
	};

	enum class BufferViewFlags : uint8_t
	{
		none = 0,
		cbv = 1,
		srv = 2,
		uav = 4
	};

	struct BufferCreationDesc
	{
		uint32_t mSize = 0;
		uint32_t mStride = 0;
		BufferViewFlags mViewFlags = BufferViewFlags::none;
		BufferAccessFlags mAccessFlags = BufferAccessFlags::gpuOnly;
		bool mIsRawAccess = false;
	};

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
		GPUBuffer();

		void SetMappedData(const void* data, size_t dataSize);

		friend class UploadContext;

	protected:
		uint8_t* mMappedResource = nullptr;
		uint32_t mStride = 0;
		DescriptorHandle mCBVDescriptor{};
		DescriptorHandle mSRVDescriptor{};
		DescriptorHandle mUAVDescriptor{};
	};
}

