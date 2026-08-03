#pragma once

#include "D3DHelpers.h" // Asumo que aquí tienes tus includes de DX12
#include <wrl/client.h>
#include <cstdint>
#include <stdexcept>

namespace Bruno
{
    class GraphicsDevice;
    
    struct DynamicAllocation
    {
        void* CPUAddress;
        D3D12_GPU_VIRTUAL_ADDRESS GPUAddress;
    };
    
    class LinearAllocator
    {
    public:
        LinearAllocator(GraphicsDevice& device, size_t pageSize = 1024 * 1024 * 2); // 2MB por defecto
        ~LinearAllocator();

        DynamicAllocation Allocate(size_t size);

        // Se llama al inicio de cada frame para reciclar la memoria
        void Reset();

    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> m_buffer;
        void* m_mappedPtr;
        D3D12_GPU_VIRTUAL_ADDRESS m_gpuAddress;
        size_t m_offset;
        size_t m_pageSize;
    };
    
    struct FrameResource
    {
        // El Allocator de comandos que ya tienes
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandAllocator;
    
        // NUEVO: Nuestro buffer pre-reservado para data dinámica (Upload Heap persistente)
        std::unique_ptr<LinearAllocator> DynamicAllocator;
    
        // Fence value para saber cuándo la GPU terminó con ESTE frame específicamente
        uint64_t FenceValue = 0;
    };
}