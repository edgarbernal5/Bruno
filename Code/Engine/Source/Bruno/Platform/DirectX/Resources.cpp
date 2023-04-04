#include "brpch.h"
#include "Resources.h"

#include "GraphicsDevice.h"

namespace Bruno
{
    DescriptorHandle DescriptorHeap::Allocate()
    {
        //std::lock_guard lock{ m_mutex };

        const uint32_t index{ m_size };
        const uint32_t offset{ index * m_descriptor_size };
        ++m_size;

        DescriptorHandle handle;
        handle.Cpu.ptr = m_cpu_start.ptr + offset;

        handle.Index = index;

        return handle;
    }

    bool DescriptorHeap::Initialize(GraphicsDevice* device)
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc{};
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NumDescriptors = 512;//Graphics::Core::FRAME_BUFFER_COUNT;
        desc.Type = m_type;
        desc.NodeMask = 0;

        HRESULT hr{ S_OK };
        hr = device->GetD3DDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap));
        if (FAILED(hr)) return false;

        m_size = 0;
        m_descriptor_size = device->GetD3DDevice()->GetDescriptorHandleIncrementSize(m_type);
        m_cpu_start = m_heap->GetCPUDescriptorHandleForHeapStart();
        m_gpu_start = D3D12_GPU_DESCRIPTOR_HANDLE{ 0 };

        return true;
    }
}