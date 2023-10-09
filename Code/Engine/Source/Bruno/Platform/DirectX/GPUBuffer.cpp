#include "brpch.h"
#include "GPUBuffer.h"

#include "GraphicsDevice.h"

#include "Bruno/Core/Memory.h"
#include "D3D12MemAlloc.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(GPUBuffer);

	GPUBuffer::GPUBuffer(const BufferCreationDesc& desc)
	{
        GraphicsDevice* device = Graphics::GetDevice();
		mType = GPUResourceType::Buffer;

		mDesc.Width = AlignU32(static_cast<uint32_t>(desc.mSize), 256);
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
		mStride = desc.mStride;

        uint32_t numElements = static_cast<uint32_t>(mStride > 0 ? desc.mSize / mStride : 1);
        bool isHostVisible = ((desc.mAccessFlags & BufferAccessFlags::hostWritable) == BufferAccessFlags::hostWritable);
        bool hasCBV = ((desc.mViewFlags & BufferViewFlags::cbv) == BufferViewFlags::cbv);
        bool hasSRV = ((desc.mViewFlags & BufferViewFlags::srv) == BufferViewFlags::srv);
        bool hasUAV = ((desc.mViewFlags & BufferViewFlags::uav) == BufferViewFlags::uav);

        D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COPY_DEST;

        if (isHostVisible)
        {
            resourceState = D3D12_RESOURCE_STATE_GENERIC_READ;
        }

        mState = resourceState;

        D3D12MA::ALLOCATION_DESC allocationDesc{};
        allocationDesc.HeapType = isHostVisible ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT;

        device->GetAllocator()->CreateResource(&allocationDesc, &mDesc, resourceState, nullptr, &mAllocation, IID_PPV_ARGS(&mResource));
        mVirtualAddress = mResource->GetGPUVirtualAddress();

        if (hasCBV)
        {
            D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDesc = {};
            constantBufferViewDesc.BufferLocation = mResource->GetGPUVirtualAddress();
            constantBufferViewDesc.SizeInBytes = static_cast<uint32_t>(mDesc.Width);

            //mCBVDescriptor = mSRVStagingDescriptorHeap->GetNewDescriptor();
            //mDevice->CreateConstantBufferView(&constantBufferViewDesc, mCBVDescriptor.mCPUHandle);
        }

        if (hasSRV)
        {
            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.Format = desc.mIsRawAccess ? DXGI_FORMAT_R32_TYPELESS : DXGI_FORMAT_UNKNOWN;
            srvDesc.Buffer.FirstElement = 0;
            srvDesc.Buffer.NumElements = static_cast<uint32_t>(desc.mIsRawAccess ? (desc.mSize / 4) : numElements);
            srvDesc.Buffer.StructureByteStride = desc.mIsRawAccess ? 0 : mStride;
            srvDesc.Buffer.Flags = desc.mIsRawAccess ? D3D12_BUFFER_SRV_FLAG_RAW : D3D12_BUFFER_SRV_FLAG_NONE;

            //mSRVDescriptor = mSRVStagingDescriptorHeap->GetNewDescriptor();
            //mDevice->CreateShaderResourceView(mResource, &srvDesc, mSRVDescriptor.mCPUHandle);

            //mDescriptorHeapIndex = mFreeReservedDescriptorIndices.back();
            //mFreeReservedDescriptorIndices.pop_back();

            //CopySRVHandleToReservedTable(mSRVDescriptor, mDescriptorHeapIndex);
        }

        if (hasUAV)
        {
            D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
            uavDesc.Format = desc.mIsRawAccess ? DXGI_FORMAT_R32_TYPELESS : DXGI_FORMAT_UNKNOWN;
            uavDesc.Buffer.CounterOffsetInBytes = 0;
            uavDesc.Buffer.FirstElement = 0;
            uavDesc.Buffer.NumElements = static_cast<uint32_t>(desc.mIsRawAccess ? (desc.mSize / 4) : numElements);
            uavDesc.Buffer.StructureByteStride = desc.mIsRawAccess ? 0 : mStride;
            uavDesc.Buffer.Flags = desc.mIsRawAccess ? D3D12_BUFFER_UAV_FLAG_RAW : D3D12_BUFFER_UAV_FLAG_NONE;

            //mUAVDescriptor = mSRVStagingDescriptorHeap->GetNewDescriptor();
            //mDevice->CreateUnorderedAccessView(mResource, nullptr, &uavDesc, mUAVDescriptor.mCPUHandle);
        }

        if (isHostVisible)
        {
            mResource->Map(0, nullptr, reinterpret_cast<void**>(&mMappedResource));
        }
	}

	GPUBuffer::GPUBuffer()
	{
		mType = GPUResourceType::Buffer;
	}

	void GPUBuffer::SetMappedData(const void* data, size_t dataSize)
	{
		BR_ASSERT(mMappedResource != nullptr && data != nullptr && dataSize > 0 && dataSize <= mDesc.Width);
		memcpy_s(mMappedResource, mDesc.Width, data, dataSize);
	}
}