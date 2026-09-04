#pragma once
#include "Bruno/Platform/DirectX/DescriptorAllocator.h"

#include "Bruno/Renderer/MaterialData.h"

namespace Bruno
{
    class GpuBuffer;
    class UploadHeap;
    class DescriptorAllocator;
    class GraphicsContext;

    class MaterialManager
    {
    public:
        MaterialManager(GraphicsDevice& device, DescriptorAllocator& globalSrvHeap);
        
        uint32_t CreateMaterial(const MaterialData& data);
        MaterialData& GetMaterial(uint32_t index);
        void MarkAsDirty(); 
        
        // AHORA USA EL CONTEXTO DIRECTO PARA NO ROMPER EL PARALELISMO
        void UpdateGPUBuffer(GraphicsContext& context);
        

    private:
        GraphicsDevice& m_device;
        DescriptorAllocator& m_globalSrvHeap;
        
        std::vector<MaterialData> m_materials;
        bool m_isDirty = true;
        uint32_t m_gpuBufferSize = 0;

        // VRAM Pura (GpuBuffer heredaría de GraphicsResource también)
        std::unique_ptr<GpuBuffer> m_gpuBuffer;
        
        // RAM Mapeada encapsulada elegantemente
        std::unique_ptr<UploadHeap> m_stagingBuffer;

        DescriptorAllocation m_srvAllocation;
        
        void ResizeGPUBuffer(uint32_t newElementCount);
    };
}
