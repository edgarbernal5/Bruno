#pragma once

#include "D3DHelpers.h"
#include "DescriptorHandle.h"
#include <mutex>
#include <vector>
#include <optional>

namespace Bruno::DX {

    class GraphicsDevice;

    class DescriptorAllocator 
    {
    public:
        DescriptorAllocator(GraphicsDevice& device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t capacity, bool isShaderVisible = false);
        ~DescriptorAllocator() = default;

        DescriptorAllocator(const DescriptorAllocator&) = delete;
        DescriptorAllocator& operator=(const DescriptorAllocator&) = delete;

        // Devuelve un handle y su índice en el heap. 
        // Usamos std::optional (C++17) por si nos quedamos sin memoria.
        [[nodiscard]] std::optional<uint32_t> Allocate();

        // Libera un índice para que otra textura pueda usarlo
        void Free(uint32_t index);

        [[nodiscard]] DescriptorHandle GetHandle(uint32_t index) const;
        [[nodiscard]] ID3D12DescriptorHeap* GetNativeHeap() const { return m_heap.Get(); }

    private:
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_heap;
        D3D12_DESCRIPTOR_HEAP_TYPE m_heapType;
        
        uint32_t m_descriptorSize{ 0 };
        uint32_t m_capacity{ 0 };
        
        // --- Free List Management ---
        std::mutex m_allocationMutex; // Para que tu ECS pueda cargar texturas en multihilo
        std::vector<uint32_t> m_freeIndices;
        uint32_t m_currentOffset{ 0 };
    };
}