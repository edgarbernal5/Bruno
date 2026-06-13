#pragma once

#include "D3DHelpers.h" // Asumo que aquí tienes tus includes de DX12
#include <wrl/client.h>
#include <cstdint>
#include <stdexcept>

namespace Bruno::DX 
{
    // Representa un bloque de memoria concedido dentro del Heap
    struct DescriptorAllocation 
    {
        D3D12_CPU_DESCRIPTOR_HANDLE CPU;
        D3D12_GPU_DESCRIPTOR_HANDLE GPU;
        uint32_t Index;
    };

    class DescriptorAllocator 
    {
    public:
        // Por defecto, 256 descriptores es un excelente inicio para un CBV_SRV_UAV heap
        DescriptorAllocator(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t capacity = 256);
        ~DescriptorAllocator() = default;

        // Prohibimos copias para proteger el recurso COM nativo
        DescriptorAllocator(const DescriptorAllocator&) = delete;
        DescriptorAllocator& operator=(const DescriptorAllocator&) = delete;

        // [[nodiscard]] asegura que no asignemos memoria al vacío por accidente
        [[nodiscard]] DescriptorAllocation Allocate(uint32_t count = 1);
        
        ID3D12DescriptorHeap* GetHeap() const { return m_heap.Get(); }
        uint32_t GetCapacity() const { return m_capacity; }

    private:
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_heap;
        D3D12_DESCRIPTOR_HEAP_TYPE m_heapType;
        
        uint32_t m_descriptorSize;
        uint32_t m_capacity;
        uint32_t m_allocatedCount;

        D3D12_CPU_DESCRIPTOR_HANDLE m_cpuStart;
        D3D12_GPU_DESCRIPTOR_HANDLE m_gpuStart;
    };
}