#include "brpch.h"
#include "Resources.h"

#include "GraphicsDevice.h"

namespace Bruno
{
    DescriptorHeap::DescriptorHeap(GraphicsDevice& device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t maxDescriptors, bool isShaderVisible) :
        m_heapType(heapType)
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc{};
        desc.Flags = isShaderVisible
            ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
            : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NumDescriptors = maxDescriptors;
        desc.Type = m_heapType;
        desc.NodeMask = 0;

        ThrowIfFailed(device.GetD3DDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap)));

        m_maxDescriptors = maxDescriptors;
        m_currentDescriptorIndex = 0;

        m_cpuStart = m_heap->GetCPUDescriptorHandleForHeapStart();
        m_gpuStart = isShaderVisible ?
            m_heap->GetGPUDescriptorHandleForHeapStart() : D3D12_GPU_DESCRIPTOR_HANDLE{ 0 };

        m_descriptorSize = device.GetD3DDevice()->GetDescriptorHandleIncrementSize(m_heapType);
    }

    DescriptorHeap::~DescriptorHeap()
    {
        SafeRelease(m_heap);
    }

    StagingDescriptorHeap::StagingDescriptorHeap(GraphicsDevice& device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t numDescriptors) :
        DescriptorHeap(device, heapType, numDescriptors, false)
    {
        m_freeDescriptors.reserve(numDescriptors);
    }

    StagingDescriptorHeap::~StagingDescriptorHeap()
    {
        if (m_activeHandleCount != 0)
        {
            BR_ASSERT_ERROR("There were active handles when the descriptor heap was destroyed. Look for leaks.");
        }
    }

    DescriptorHandle StagingDescriptorHeap::Allocate(uint32_t count)
    {
        //std::lock_guard lock{ m_mutex };
        BR_ASSERT(m_heap);
        uint32_t newHandleID = 0;

        if (m_currentDescriptorIndex < m_maxDescriptors)
        {
            newHandleID = m_currentDescriptorIndex;
            ++m_currentDescriptorIndex;
        }
        else if (m_freeDescriptors.size() > 0)
        {
            newHandleID = m_freeDescriptors.back();
            m_freeDescriptors.pop_back();
        }
        else
        {
            BR_ASSERT_ERROR("Need to increase heap size.");
        }

        uint32_t offset = newHandleID * m_descriptorSize;

        DescriptorHandle handle;
        handle.HeapIndex = newHandleID;

        handle.Cpu.ptr = m_cpuStart.ptr + static_cast<uint64_t>(offset);
        handle.Gpu.ptr = m_gpuStart.ptr;
        if (IsShaderVisible())
        {
            handle.Gpu.ptr += static_cast<uint64_t>(offset);
        }

        ++m_activeHandleCount;
        return handle;
    }
    
    void StagingDescriptorHeap::Free(DescriptorHandle& handle)
    {
        BR_ASSERT(m_activeHandleCount > 0, "Heap is empty, no handles to be freed.");
        m_freeDescriptors.push_back(handle.HeapIndex);

        m_activeHandleCount--;
    }

    DescriptorHandle RenderPassDescriptorHeap::Allocate(uint32_t count)
    {
        BR_ASSERT(m_heap);
        uint32_t newHandleID = 0;
        uint32_t blockEnd = m_currentDescriptorIndex + count;

        if (blockEnd <= m_maxDescriptors) {
            newHandleID = m_currentDescriptorIndex;
            m_currentDescriptorIndex = blockEnd;
        }
        else {
            BR_ASSERT_ERROR("Need to increase heap size.");
        }

        uint64_t offset = newHandleID * m_descriptorSize;

        DescriptorHandle handle;
        handle.HeapIndex = newHandleID;

        handle.Cpu.ptr = m_cpuStart.ptr + offset;
        handle.Gpu.ptr = m_gpuStart.ptr + offset;
        
        return handle;
    }

    DescriptorHandle RenderPassDescriptorHeap::GetReservedDescriptor(uint32_t index)
    {
        BR_ASSERT(index < m_reservedHandleCount);

        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_cpuStart;
        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = m_gpuStart;
        cpuHandle.ptr += static_cast<uint64_t>(index) * m_descriptorSize;
        gpuHandle.ptr += static_cast<uint64_t>(index) * m_descriptorSize;

        DescriptorHandle descriptor;
        descriptor.HeapIndex = index;
        descriptor.Cpu = cpuHandle;
        descriptor.Gpu = gpuHandle;

        return descriptor;
    }

    void RenderPassDescriptorHeap::Reset()
    {
        m_currentDescriptorIndex = m_reservedHandleCount;
    }
}