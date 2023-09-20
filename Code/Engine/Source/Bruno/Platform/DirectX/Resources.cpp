#include "brpch.h"
#include "Resources.h"

#include "GraphicsDevice.h"

namespace Bruno
{
    DescriptorHeap::~DescriptorHeap()
    {
        SafeRelease(m_heap);
    }

    bool DescriptorHeap::Initialize(GraphicsDevice* device, uint32_t maxDescriptors, bool isShaderVisible)
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc{};
        desc.Flags = isShaderVisible
            ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
            : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NumDescriptors = maxDescriptors;
        desc.Type = m_heapType;
        desc.NodeMask = 0;

        HRESULT hr{ S_OK };
        hr = device->GetD3DDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap));
        if (FAILED(hr)) return false;

        m_maxDescriptors = maxDescriptors;
        m_currentDescriptorIndex = 0;

        m_cpuStart = m_heap->GetCPUDescriptorHandleForHeapStart();
        m_gpuStart = isShaderVisible ?
            m_heap->GetGPUDescriptorHandleForHeapStart() : D3D12_GPU_DESCRIPTOR_HANDLE{ 0 };
        m_descriptorSize = device->GetD3DDevice()->GetDescriptorHandleIncrementSize(m_heapType);

        return true;
    }

    StagingDescriptorHeap::~StagingDescriptorHeap()
    {
    }

    DescriptorHandle StagingDescriptorHeap::Allocate(uint32_t count)
    {
        //std::lock_guard lock{ m_mutex };
        BR_ASSERT(m_heap);
        uint32_t newHandleID = 0;

        if (m_currentDescriptorIndex < m_maxDescriptors) {
            newHandleID = m_currentDescriptorIndex;
            ++m_currentDescriptorIndex;
        }
        else if (mFreeDescriptors.size() > 0) {
            newHandleID = mFreeDescriptors.back();
            mFreeDescriptors.pop_back();
        }
        else {
            BR_ASSERT(false, "Need to increase heap size.");
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

        mActiveHandleCount++;
        return handle;
    }
    
    void StagingDescriptorHeap::Free(DescriptorHandle handle)
    {
        BR_ASSERT(mActiveHandleCount > 0, "Heap is empty, no handles to be freed.");
        mFreeDescriptors.push_back(handle.HeapIndex);

        mActiveHandleCount--;
    }
}