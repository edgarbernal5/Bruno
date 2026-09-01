#pragma once
#include "Bruno/Platform/DirectX/DescriptorAllocator.h"

namespace Bruno
{
    class DescriptorAllocator;
    class GraphicsContext;
    struct MaterialData;

    class MaterialManager
    {
    public:
        MaterialManager(GraphicsDevice& device, DescriptorAllocator& globalSrvHeap);
        
        uint32_t CreateMaterial(const MaterialData& data);
        MaterialData& GetMaterial(uint32_t index);
        void MarkAsDirty(); 
        
        // AHORA USA EL CONTEXTO DIRECTO PARA NO ROMPER EL PARALELISMO
        void UpdateGPUBuffer(GraphicsContext& context);
        
        ID3D12Resource* GetGPUBuffer() const { return m_gpuBuffer.Get(); }

    private:
        GraphicsDevice& m_device;
        DescriptorAllocator& m_globalSrvHeap;
        
        std::vector<MaterialData> m_materials;
        bool m_isDirty = true;

        // El gemelo de alto rendimiento (VRAM)
        Microsoft::WRL::ComPtr<ID3D12Resource> m_gpuBuffer;
        
        // El gemelo de alta accesibilidad (RAM mapeada)
        Microsoft::WRL::ComPtr<ID3D12Resource> m_stagingBuffer;
        void* m_mappedStagingData = nullptr; // Puntero permanente a la RAM

        DescriptorAllocation m_srvAllocation;
        uint32_t m_gpuBufferSize = 0;
        
        void ResizeGPUBuffer(uint32_t newElementCount);
    };
}
