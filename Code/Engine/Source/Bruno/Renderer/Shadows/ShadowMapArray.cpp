#include "brpch.h"
#include "ShadowMapArray.h"

#include "Bruno/Platform/DirectX/GraphicsDevice.h"

namespace Bruno
{
    void ShadowMapArray::Initialize(GraphicsDevice* device, uint32_t resolution, uint32_t numCascades)
    {
        m_width = resolution;
        m_height = resolution;
        m_numCascades = numCascades;

        auto d3dDevice = device->GetNativeDevice();

        // 1. Crear el Recurso como TYPELESS
        // ArraySize es numCascades (4)
        CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
            DXGI_FORMAT_R32_TYPELESS, // ¡Importante! No usar D32_FLOAT aquí
            m_width, m_height, 
            m_numCascades, 
            1, 1, 0, 
            D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
        );

        // 2. El Clear Value DEBE coincidir con el formato del DSV, no del recurso
        CD3DX12_CLEAR_VALUE clearValue(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);

        // Alojamos la memoria en la VRAM (Default Heap)
        CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
        d3dDevice->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE, // Estado inicial
            &clearValue,
            IID_PPV_ARGS(&m_resource)
        );
            
        //CreateViews(device);
    }
}
