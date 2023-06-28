#include "brpch.h"
#include "Resources.h"

#include "GraphicsDevice.h"

namespace Bruno
{
    DescriptorHandle DescriptorHeap::Allocate()
    {
        //std::scoped_lock lock{ m_mutex };
        BR_ASSERT(m_heap);
        BR_ASSERT(m_size < m_capacity);

        const uint32_t index{ m_size };
        const uint32_t offset{ index * m_descriptorSize };
        ++m_size;

        DescriptorHandle handle;
        handle.Cpu.ptr = m_cpuStart.ptr + offset;
        if (IsShaderVisible())
        {
            handle.Gpu.ptr = m_gpuStart.ptr + offset;
        }
        handle.Index = index;

        return handle;
    }

    bool DescriptorHeap::Initialize(GraphicsDevice* device, uint32_t capacity, bool isShaderVisible)
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc{};
        desc.Flags = isShaderVisible
            ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
            : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NumDescriptors = capacity;
        desc.Type = m_type;
        desc.NodeMask = 0;

        HRESULT hr{ S_OK };
        hr = device->GetD3DDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap));
        if (FAILED(hr)) return false;

        m_capacity = capacity;
        m_size = 0;
        m_descriptorSize = device->GetD3DDevice()->GetDescriptorHandleIncrementSize(m_type);
        m_cpuStart = m_heap->GetCPUDescriptorHandleForHeapStart();
        m_gpuStart = isShaderVisible ?
            m_heap->GetGPUDescriptorHandleForHeapStart() : D3D12_GPU_DESCRIPTOR_HANDLE{ 0 };

        return true;
    }
}