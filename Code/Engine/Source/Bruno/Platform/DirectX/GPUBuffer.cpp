#include "brpch.h"
#include "GPUBuffer.h"

#include "GraphicsDevice.h"

#include "Bruno/Core/Memory.h"
#include "D3D12MemAlloc.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(GPUBuffer);

	GPUBuffer::GPUBuffer(GraphicsDevice& device, const BufferCreationDesc& creationDesc)
	{
		mType = GPUResourceType::Buffer;

        mDesc.Width = creationDesc.mSize;
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
		mStride = creationDesc.mStride;

        uint32_t numElements = static_cast<uint32_t>(mStride > 0 ? creationDesc.mSize / mStride : 1);
        bool isHostVisible = ((creationDesc.mAccessFlags & BufferAccessFlags::hostWritable) == BufferAccessFlags::hostWritable);
        bool hasCBV = ((creationDesc.mViewFlags & BufferViewFlags::cbv) == BufferViewFlags::cbv);
        bool hasSRV = ((creationDesc.mViewFlags & BufferViewFlags::srv) == BufferViewFlags::srv);
        bool hasUAV = ((creationDesc.mViewFlags & BufferViewFlags::uav) == BufferViewFlags::uav);

        D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COPY_DEST;

        if (isHostVisible)
        {
            resourceState = D3D12_RESOURCE_STATE_GENERIC_READ;
        }

        mState = resourceState;

        D3D12MA::ALLOCATION_DESC allocationDesc{};
        allocationDesc.HeapType = isHostVisible ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT;

        device.GetAllocator()->CreateResource(&allocationDesc, &mDesc, resourceState, nullptr, &mAllocation, IID_PPV_ARGS(&mResource));
        mVirtualAddress = mResource->GetGPUVirtualAddress();

        if (hasCBV)
        {
            D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDesc = {};
            constantBufferViewDesc.BufferLocation = mResource->GetGPUVirtualAddress();
            constantBufferViewDesc.SizeInBytes = static_cast<uint32_t>(mDesc.Width);

            mCBVDescriptor = device.GetSrvDescriptionHeap().Allocate();
            device.GetD3DDevice()->CreateConstantBufferView(&constantBufferViewDesc, mCBVDescriptor.Cpu);
        }

        if (hasSRV)
        {
            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.Format = creationDesc.mIsRawAccess ? DXGI_FORMAT_R32_TYPELESS : DXGI_FORMAT_UNKNOWN;
            srvDesc.Buffer.FirstElement = 0;
            srvDesc.Buffer.NumElements = static_cast<uint32_t>(creationDesc.mIsRawAccess ? (creationDesc.mSize / 4) : numElements);
            srvDesc.Buffer.StructureByteStride = creationDesc.mIsRawAccess ? 0 : mStride;
            srvDesc.Buffer.Flags = creationDesc.mIsRawAccess ? D3D12_BUFFER_SRV_FLAG_RAW : D3D12_BUFFER_SRV_FLAG_NONE;

            mSRVDescriptor = device.GetSrvDescriptionHeap().Allocate();
            device.GetD3DDevice()->CreateShaderResourceView(mResource, &srvDesc, mSRVDescriptor.Cpu);

            mDescriptorHeapIndex = device.GetFreeReservedDescriptorIndex();

            device.CopySRVHandleToReservedTable(mSRVDescriptor, mDescriptorHeapIndex);
        }

        if (hasUAV)
        {
            D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
            uavDesc.Format = creationDesc.mIsRawAccess ? DXGI_FORMAT_R32_TYPELESS : DXGI_FORMAT_UNKNOWN;
            uavDesc.Buffer.CounterOffsetInBytes = 0;
            uavDesc.Buffer.FirstElement = 0;
            uavDesc.Buffer.NumElements = static_cast<uint32_t>(creationDesc.mIsRawAccess ? (creationDesc.mSize / 4) : numElements);
            uavDesc.Buffer.StructureByteStride = creationDesc.mIsRawAccess ? 0 : mStride;
            uavDesc.Buffer.Flags = creationDesc.mIsRawAccess ? D3D12_BUFFER_UAV_FLAG_RAW : D3D12_BUFFER_UAV_FLAG_NONE;

            mUAVDescriptor = device.GetSrvDescriptionHeap().Allocate();
            device.GetD3DDevice()->CreateUnorderedAccessView(mResource, nullptr, &uavDesc, mUAVDescriptor.Cpu);
        }

        if (isHostVisible)
        {
            mResource->Map(0, nullptr, reinterpret_cast<void**>(&mMappedResource));
        }
	}

    GPUBuffer::~GPUBuffer()
    {
        auto device = Graphics::GetDevice();
        //TODO: add a new request to device to destroy the resource. we shouldn't do this here.

        if (mCBVDescriptor.IsValid())
            device->GetSrvDescriptionHeap().Free(mCBVDescriptor);

        if (mSRVDescriptor.IsValid())
            device->GetSrvDescriptionHeap().Free(mSRVDescriptor);

        if (mUAVDescriptor.IsValid())
            device->GetSrvDescriptionHeap().Free(mUAVDescriptor);
    }

	void GPUBuffer::SetMappedData(const void* data, size_t dataSize)
	{
		BR_ASSERT(mMappedResource != nullptr && data != nullptr && dataSize > 0 && dataSize <= mDesc.Width);
		memcpy_s(mMappedResource, mDesc.Width, data, dataSize);
	}
}