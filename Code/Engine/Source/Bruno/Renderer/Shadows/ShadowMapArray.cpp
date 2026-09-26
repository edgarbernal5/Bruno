#include "brpch.h"
#include "ShadowMapArray.h"

#include "Bruno/Platform/DirectX/GraphicsDevice.h"

namespace Bruno
{
    BR_RTTI_DEFINITIONS(ShadowMapArray);
    
    void ShadowMapArray::Initialize(GraphicsDevice* device, DescriptorAllocator& srvHeap, DescriptorAllocator& dsvHeap, uint32_t resolution, uint32_t numCascades)
    {
        m_resolution = resolution;
        m_numCascades = numCascades;
        m_currentState = ResourceState::DepthWrite;
        
        auto nativeDevice = device->GetNativeDevice();

        // ==========================================
        // 1. CREACIÓN DEL RECURSO (R32_TYPELESS)
        // ==========================================
        D3D12_RESOURCE_DESC texDesc = {};
        texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        texDesc.Width = resolution;
        texDesc.Height = resolution;
        texDesc.DepthOrArraySize = numCascades; // ¡El tamaño del Array coincide con el número de cascadas![cite: 1]
        texDesc.MipLevels = 1;
        texDesc.Format = DXGI_FORMAT_R32_TYPELESS; // Memoria sin tipo definido para permitir flexibilidad de lectura/escritura[cite: 1]
        texDesc.SampleDesc.Count = 1;
        texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        // El Clear Value DEBE coincidir con el formato del DSV (D32_FLOAT), no con el del recurso base
        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = DXGI_FORMAT_D32_FLOAT;
        clearValue.DepthStencil.Depth = 1.0f;
        clearValue.DepthStencil.Stencil = 0;

        auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        ThrowIfFailed(nativeDevice->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &texDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE, // Estado inicial optimizado para escribir las sombras
            &clearValue,
            IID_PPV_ARGS(&m_resource)
        ));
        m_resource->SetName(L"Cascaded_Shadow_Map_Array");
            
        // ==========================================
        // 2. CREAR LOS 4 DSVs (Uno para cada Cascada)
        // ==========================================
        m_dsvAllocations.resize(numCascades);
            
        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // Lente de Profundidad
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
        dsvDesc.Texture2DArray.ArraySize = 1; // Solo renderizamos a UNA capa a la vez durante el pase de geometría
        dsvDesc.Texture2DArray.MipSlice = 0;

        for (uint32_t i = 0; i < numCascades; ++i)
        {
            dsvDesc.Texture2DArray.FirstArraySlice = i; // Seleccionamos la capa específica 'i' de la cascada actual
                
            m_dsvAllocations[i] = dsvHeap.Allocate(1); // Pedimos hueco en el DSV Heap
            nativeDevice->CreateDepthStencilView(m_resource.Get(), &dsvDesc, m_dsvAllocations[i].GetCPUHandle());
        }

        // ==========================================
        // 3. CREAR 1 SRV (Que abarca todo el Array)
        // ==========================================
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = DXGI_FORMAT_R32_FLOAT; // Lente de Lectura para usar en el Deferred Shader[cite: 1]
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Texture2DArray.FirstArraySlice = 0;
        srvDesc.Texture2DArray.ArraySize = numCascades; // El SRV abarca las 4 capas juntas[cite: 1]
        srvDesc.Texture2DArray.MipLevels = 1;
        srvDesc.Texture2DArray.MostDetailedMip = 0;

        m_srvAllocation = srvHeap.Allocate(1); // Alocamos directamente en tu Mega Heap Bindless global[cite: 1]
        nativeDevice->CreateShaderResourceView(m_resource.Get(), &srvDesc, m_srvAllocation.GetCPUHandle());
    }
}
