#pragma once

#include "D3DHelpers.h" // Asumo que aquí tienes tus includes de DX12
#include <wrl/client.h>
#include <cstdint>
#include <stdexcept>

namespace Bruno::DX 
{
    class GraphicsDevice;
    
    // Representa un bloque de memoria concedido dentro del Heap
    struct DescriptorAllocation
    {
        D3D12_CPU_DESCRIPTOR_HANDLE CPU;
        D3D12_GPU_DESCRIPTOR_HANDLE GPU;
        uint32_t Index;
        uint32_t Count;          // Cuántos descriptores tiene este bloque
        uint32_t DescriptorSize; // Tamaño en bytes de cada descriptor

        // Obtener el handle base (offset 0)
        D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const { return CPU; }
        D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const { return GPU; }

        // Obtener un handle con desplazamiento (offset)
        D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32_t offset) const 
        {
            // Aseguramos no salirnos del bloque asignado
            if (offset >= Count)
            {
                throw std::out_of_range("Offset de descriptor fuera de rango.");
            }
        
            D3D12_CPU_DESCRIPTOR_HANDLE handle = CPU;
            handle.ptr += (static_cast<SIZE_T>(offset) * DescriptorSize);
            return handle;
        }

        D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(uint32_t offset) const 
        {
            if (offset >= Count) 
            {
                throw std::out_of_range("Offset de descriptor fuera de rango.");
            }
            
            D3D12_GPU_DESCRIPTOR_HANDLE handle = GPU;
            handle.ptr += (static_cast<SIZE_T>(offset) * DescriptorSize);
            return handle;
        }
    };

    //DescriptorAllocator o DescriptorHeap o DescriptorHeapAllocator
    class DescriptorAllocator
    {
    public:
        // NUEVO: Constructor que inicializa el Heap
        DescriptorAllocator(DX::GraphicsDevice& device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t capacity, bool isShaderVisible);
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