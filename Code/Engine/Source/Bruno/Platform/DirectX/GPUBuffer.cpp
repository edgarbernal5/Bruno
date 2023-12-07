#include "brpch.h"
#include "GpuBuffer.h"

#include "GraphicsDevice.h"

#include "Bruno/Core/Memory.h"
#include "D3D12MemAlloc.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(GpuBuffer);

	GpuBuffer::GpuBuffer(GraphicsDevice& device, const BufferCreationDesc& creationDesc)
	{
		m_resourceType = GPUResourceType::Buffer;

        m_desc.Width = creationDesc.Size;
		m_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		m_desc.Alignment = 0;
		m_desc.Height = 1;
		m_desc.DepthOrArraySize = 1;
		m_desc.MipLevels = 1;
		m_desc.Format = DXGI_FORMAT_UNKNOWN;
		m_desc.SampleDesc.Count = 1;
		m_desc.SampleDesc.Quality = 0;
		m_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		m_desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		m_stride = creationDesc.Stride;

        m_numElements = static_cast<uint32_t>(m_stride > 0 ? creationDesc.Size / m_stride : 1);
        bool isHostVisible = ((creationDesc.AccessFlags & BufferAccessFlags::HostWritable) == BufferAccessFlags::HostWritable);
        bool hasCBV = ((creationDesc.ViewFlags & BufferViewFlags::Cbv) == BufferViewFlags::Cbv);
        bool hasSRV = ((creationDesc.ViewFlags & BufferViewFlags::Srv) == BufferViewFlags::Srv);
        bool hasUAV = ((creationDesc.ViewFlags & BufferViewFlags::Uav) == BufferViewFlags::Uav);

        D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COPY_DEST;

        if (isHostVisible)
        {
            resourceState = D3D12_RESOURCE_STATE_GENERIC_READ;
        }

        m_state = resourceState;

        D3D12MA::ALLOCATION_DESC allocationDesc{};
        allocationDesc.HeapType = isHostVisible ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT;

        device.GetAllocator()->CreateResource(&allocationDesc, &m_desc, resourceState, nullptr, &m_allocation, IID_PPV_ARGS(&m_resource));
        m_virtualAddress = m_resource->GetGPUVirtualAddress();

        if (hasCBV)
        {
            D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDesc = {};
            constantBufferViewDesc.BufferLocation = m_resource->GetGPUVirtualAddress();
            constantBufferViewDesc.SizeInBytes = static_cast<uint32_t>(m_desc.Width);

            m_cbvDescriptor = device.GetSrvDescriptionHeap().Allocate();
            device.GetD3DDevice()->CreateConstantBufferView(&constantBufferViewDesc, m_cbvDescriptor.Cpu);
        }

        if (hasSRV)
        {
            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.Format = creationDesc.IsRawAccess ? DXGI_FORMAT_R32_TYPELESS : DXGI_FORMAT_UNKNOWN;
            srvDesc.Buffer.FirstElement = 0;
            srvDesc.Buffer.NumElements = static_cast<uint32_t>(creationDesc.IsRawAccess ? (creationDesc.Size / 4) : m_numElements);
            srvDesc.Buffer.StructureByteStride = creationDesc.IsRawAccess ? 0 : m_stride;
            srvDesc.Buffer.Flags = creationDesc.IsRawAccess ? D3D12_BUFFER_SRV_FLAG_RAW : D3D12_BUFFER_SRV_FLAG_NONE;

            m_srvDescriptor = device.GetSrvDescriptionHeap().Allocate();
            device.GetD3DDevice()->CreateShaderResourceView(m_resource, &srvDesc, m_srvDescriptor.Cpu);

            m_descriptorHeapIndex = device.GetFreeReservedDescriptorIndex();

            device.CopySRVHandleToReservedTable(m_srvDescriptor, m_descriptorHeapIndex);
        }

        if (hasUAV)
        {
            D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
            uavDesc.Format = creationDesc.IsRawAccess ? DXGI_FORMAT_R32_TYPELESS : DXGI_FORMAT_UNKNOWN;
            uavDesc.Buffer.CounterOffsetInBytes = 0;
            uavDesc.Buffer.FirstElement = 0;
            uavDesc.Buffer.NumElements = static_cast<uint32_t>(creationDesc.IsRawAccess ? (creationDesc.Size / 4) : m_numElements);
            uavDesc.Buffer.StructureByteStride = creationDesc.IsRawAccess ? 0 : m_stride;
            uavDesc.Buffer.Flags = creationDesc.IsRawAccess ? D3D12_BUFFER_UAV_FLAG_RAW : D3D12_BUFFER_UAV_FLAG_NONE;

            m_uavDescriptor = device.GetSrvDescriptionHeap().Allocate();
            device.GetD3DDevice()->CreateUnorderedAccessView(m_resource, nullptr, &uavDesc, m_uavDescriptor.Cpu);
        }

        if (isHostVisible)
        {
            m_resource->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedResource));
        }
        m_isHostVisible = isHostVisible;
	}

    GpuBuffer::~GpuBuffer()
    {
        auto device = Graphics::GetDevice();
        //TODO: add a new request to device to destroy the resource. we shouldn't do this here.

        if (m_cbvDescriptor.IsValid())
            device->GetSrvDescriptionHeap().Free(m_cbvDescriptor);

        if (m_srvDescriptor.IsValid())
            device->GetSrvDescriptionHeap().Free(m_srvDescriptor);

        if (m_uavDescriptor.IsValid())
            device->GetSrvDescriptionHeap().Free(m_uavDescriptor);
    }

    void GpuBuffer::Reset()
    {
        if (m_isHostVisible) {

            if (m_resource != nullptr)
                m_resource->Unmap(0, nullptr);

            m_resource->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedResource));
        }
    }

    void GpuBuffer::SetMappedData(const void* data, size_t dataSize)
	{
		BR_ASSERT(m_mappedResource != nullptr && data != nullptr && dataSize > 0 && dataSize <= m_desc.Width);
		memcpy_s(m_mappedResource, m_desc.Width, data, dataSize);
	}
}