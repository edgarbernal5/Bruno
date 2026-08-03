#include "brpch.h"
#include "DynamicAllocation.h"

#include "GraphicsDevice.h"

namespace Bruno
{
    LinearAllocator::LinearAllocator(GraphicsDevice& device, size_t pageSize) 
                : m_pageSize(pageSize), m_offset(0), m_mappedPtr(nullptr)
    {
        auto nativeDevice = device.GetNativeDevice();

        auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(m_pageSize);

        // Nace en GENERIC_READ. Nunca transiciona, es exclusivo de Upload.
        ThrowIfFailed(nativeDevice->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_buffer)));

        // Mapeo persistente: Se queda en memoria viva de la CPU
        ThrowIfFailed(m_buffer->Map(0, nullptr, &m_mappedPtr));
        m_gpuAddress = m_buffer->GetGPUVirtualAddress();
    }
    
    LinearAllocator::~LinearAllocator()
    {
        if (m_buffer)
        {
            m_buffer->Unmap(0, nullptr);
        }
    }

    DynamicAllocation LinearAllocator::Allocate(size_t size)
    {
        // !Importante en D3D12: Los Constant Buffers DEBEN estar alineados a 256 bytes.
        size_t alignment = 256;
        size_t alignedSize = (size + (alignment - 1)) & ~(alignment - 1);

        // TODO: En producción, deberías verificar si m_offset + alignedSize > m_pageSize
        // y generar un nuevo bloque (page) si te quedas sin espacio.

        DynamicAllocation alloc;
        alloc.CPUAddress = static_cast<uint8_t*>(m_mappedPtr) + m_offset;
        alloc.GPUAddress = m_gpuAddress + m_offset;

        m_offset += alignedSize;

        return alloc;
    }

    void LinearAllocator::Reset()
    {
        m_offset = 0;
    }
}

