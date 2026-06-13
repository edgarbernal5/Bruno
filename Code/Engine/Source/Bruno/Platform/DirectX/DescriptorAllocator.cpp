#include "brpch.h"
#include "DescriptorAllocator.h"

#include "Device.h"

namespace Bruno::DX {

    DescriptorAllocator2::DescriptorAllocator2(GraphicsDevice& device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t capacity, bool isShaderVisible)
        : m_heapType(type), m_capacity(capacity)
    {
        auto nativeDevice = device.GetNativeDevice();
        m_descriptorSize = nativeDevice->GetDescriptorHandleIncrementSize(type);

        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = type;
        desc.NumDescriptors = capacity;
        desc.Flags = isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NodeMask = 0;

        ThrowIfFailed(nativeDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap)));
    }

    std::optional<uint32_t> DescriptorAllocator2::Allocate() 
    {
        // C++17: Mutex locking seguro. Si falla algo, se libera solo.
        std::scoped_lock lock(m_allocationMutex);

        // Estrategia 1: Reutilizar espacios que han sido liberados (Free List)
        if (!m_freeIndices.empty()) 
        {
            uint32_t index = m_freeIndices.back();
            m_freeIndices.pop_back();
            return index;
        }

        // Estrategia 2: Si no hay libres, consumimos espacio nuevo (Linear Allocation)
        if (m_currentOffset < m_capacity) 
        {
            uint32_t index = m_currentOffset;
            m_currentOffset++;
            return index;
        }

        // Out of Memory (OOM) en este Heap
        // En un motor AAA real, aquí se crearía una "Página" nueva automáticamente.
        return std::nullopt; 
    }

    void DescriptorAllocator2::Free(uint32_t index) 
    {
        if (index >= m_capacity) return; // Prevención de crash

        std::scoped_lock lock(m_allocationMutex);
        
        // Guardamos el índice para reciclarlo cuando se asigne una nueva textura
        m_freeIndices.push_back(index);
    }

    DescriptorHandle DescriptorAllocator2::GetHandle(uint32_t index) const 
    {
        DescriptorHandle handle;
        if (index >= m_capacity) return handle; // Retorna un handle inválido

        // Aritmética de punteros para saber exactamente en qué byte vive este descriptor
        handle.CPU.ptr = m_heap->GetCPUDescriptorHandleForHeapStart().ptr + (index * m_descriptorSize);

        if (m_heap->GetDesc().Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) 
        {
            handle.GPU.ptr = m_heap->GetGPUDescriptorHandleForHeapStart().ptr + (index * m_descriptorSize);
        }

        return handle;
    }
}