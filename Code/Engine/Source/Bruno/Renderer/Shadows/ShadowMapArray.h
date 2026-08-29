#pragma once
#include <memory>

#include "d3dx12.h"

namespace Bruno
{

    class GraphicsDevice;
    
    class ShadowMapArray
    {

    public:
        void Initialize(GraphicsDevice* device, uint32_t resolution, uint32_t numCascades = 4);
        
        // ... getters para los handles ...
    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;
        
        // Asumiendo que tienes una clase para manejar tus descriptores (Heaps)
        D3D12_CPU_DESCRIPTOR_HANDLE m_dsvHandles[4]; 
        D3D12_CPU_DESCRIPTOR_HANDLE m_srvHandle;     

        uint32_t m_width;
        uint32_t m_height;
        uint32_t m_numCascades;
    };
}