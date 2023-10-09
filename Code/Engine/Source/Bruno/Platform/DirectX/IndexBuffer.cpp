#include "brpch.h"
#include "IndexBuffer.h"

#include "GraphicsDevice.h"
#include "UploadCommand.h"
#include "Bruno/Core/Memory.h"

#include "D3D12MemAlloc.h"

namespace Bruno
{
    IndexBuffer::IndexBuffer(uint32_t sizeInBytes, const void* bufferData, uint32_t stride) :
		GPUBuffer(),
		//m_numElements(numElements),
		m_elementSize(stride),
		m_indexBufferView{}
	{
		mDesc.Width = AlignU32(static_cast<uint32_t>(sizeInBytes), 256);
		mDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		mDesc.Alignment = 0;
		mDesc.Height = 1;
		mDesc.DepthOrArraySize = 1;
		mDesc.MipLevels = 1;
		mDesc.Format = DXGI_FORMAT_UNKNOWN;
		mDesc.SampleDesc.Count = 1;
		mDesc.SampleDesc.Quality = 0;
		mDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		mDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		mStride = stride;

		uint32_t numElements = static_cast<uint32_t>(mStride > 0 ? sizeInBytes / mStride : 1);
		bool isHostVisible = true;// ((desc.mAccessFlags & BufferAccessFlags::hostWritable) == BufferAccessFlags::hostWritable);
		bool hasCBV = false;//((desc.mViewFlags & BufferViewFlags::cbv) == BufferViewFlags::cbv);
		bool hasSRV = false;//((desc.mViewFlags & BufferViewFlags::srv) == BufferViewFlags::srv);
		bool hasUAV = false;//((desc.mViewFlags & BufferViewFlags::uav) == BufferViewFlags::uav);

		GraphicsDevice* device = Graphics::GetDevice();

		D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COPY_DEST;

		if (isHostVisible)
		{
			resourceState = D3D12_RESOURCE_STATE_GENERIC_READ;
		}
		mState = resourceState;

		// Upload index buffer data.
		D3D12MA::ALLOCATION_DESC allocationDesc{};
		allocationDesc.HeapType = isHostVisible ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT;

		device->GetAllocator()->CreateResource(&allocationDesc, &mDesc, resourceState, nullptr, &mAllocation, IID_PPV_ARGS(&mResource));
		mVirtualAddress = mResource->GetGPUVirtualAddress();

		if (isHostVisible)
		{
			mResource->Map(0, nullptr, reinterpret_cast<void**>(&mMappedResource));

			SetMappedData(bufferData, sizeInBytes);
		}

		// Create the index buffer view.
		m_indexBufferView.BufferLocation = mResource->GetGPUVirtualAddress();
        m_indexBufferView.Format = stride == sizeof(uint16_t) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
        m_indexBufferView.SizeInBytes = static_cast<uint32_t>(mDesc.Width);
	}
}