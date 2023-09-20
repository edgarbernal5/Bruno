#include "brpch.h"
#include "Resources.h"

#include "GraphicsDevice.h"

namespace Bruno
{
    DescriptorHandle DescriptorHeap::Allocate(uint32_t count)
    {
        uint32_t newHandleID = 0;
        //std::lock_guard lock{ m_mutex };
        BR_ASSERT(m_heap);
        BR_ASSERT(m_size + count < m_maxDescriptors);

        newHandleID = m_size;
        uint32_t offset = newHandleID * m_descriptorSize;
        ++m_size;

        DescriptorHandle handle;
        handle.HeapIndex = newHandleID;
        
        handle.Cpu.ptr = m_cpuStart.ptr + static_cast<uint64_t>(offset);
        handle.Gpu.ptr = m_gpuStart.ptr;
        if (IsShaderVisible())
        {
            handle.Gpu.ptr += static_cast<uint64_t>(offset);
        }

        return handle;
    }

    bool DescriptorHeap::Initialize(GraphicsDevice* device, uint32_t maxDescriptors, bool isShaderVisible)
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc{};
        desc.Flags = isShaderVisible
            ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
            : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NumDescriptors = maxDescriptors;
        desc.Type = m_type;
        desc.NodeMask = 0;

        HRESULT hr{ S_OK };
        hr = device->GetD3DDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap));
        if (FAILED(hr)) return false;

        m_maxDescriptors = maxDescriptors;
        m_size = 0;

        m_cpuStart = m_heap->GetCPUDescriptorHandleForHeapStart();
        m_gpuStart = isShaderVisible ?
            m_heap->GetGPUDescriptorHandleForHeapStart() : D3D12_GPU_DESCRIPTOR_HANDLE{ 0 };
        m_descriptorSize = device->GetD3DDevice()->GetDescriptorHandleIncrementSize(m_type);

        return true;
    }
}