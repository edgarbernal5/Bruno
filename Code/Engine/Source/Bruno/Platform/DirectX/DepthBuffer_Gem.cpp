#include "brpch.h"
#include "DepthBuffer_Gem.h"

namespace Bruno::DX {
    DepthBuffer::DepthBuffer(GraphicsDevice& device, uint32_t width, uint32_t height)
        : m_device(device), m_width(width), m_height(height) 
    {
        auto nativeDevice = m_device.GetNativeDevice();

        // 1. Crear el Descriptor Heap para el Depth Stencil View (DSV)
        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
        dsvHeapDesc.NumDescriptors = 1;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE; // El DSV no es visible a los shaders de forma directa
        
        if (FAILED(nativeDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)))) {
            throw std::runtime_error("Fallo al crear el DSV Descriptor Heap.");
        }

        CreateResourceAndDescriptor();
    }

    void DepthBuffer::Resize(uint32_t width, uint32_t height) {
        if (m_width == width && m_height == height) return;
        m_width = width;
        m_height = height;
        
        // Liberar la textura actual antes de redimensionar
        m_depthTexture.Reset();
        CreateResourceAndDescriptor();
    }

    void DepthBuffer::CreateResourceAndDescriptor() {
        auto nativeDevice = m_device.GetNativeDevice();

        // 2. Describir la memoria de la textura
        D3D12_RESOURCE_DESC depthDesc = {};
        depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        depthDesc.Width = m_width;
        depthDesc.Height = m_height;
        depthDesc.DepthOrArraySize = 1;
        depthDesc.MipLevels = 1;
        depthDesc.Format = DXGI_FORMAT_D32_FLOAT; // 32 bits para precisión de profundidad
        depthDesc.SampleDesc.Count = 1;           // Sin Anti-Aliasing (MSAA) por ahora
        depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // ¡Vital para que funcione como Depth Buffer!

        // Optimizamos la limpieza (Clear) indicando el valor por defecto (1.0f = lo más lejano)
        D3D12_CLEAR_VALUE optClear = {};
        optClear.Format = DXGI_FORMAT_D32_FLOAT;
        optClear.DepthStencil.Depth = 1.0f;
        optClear.DepthStencil.Stencil = 0;

        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM ultra rápida

        // 3. Crear el recurso en VRAM
        if (FAILED(nativeDevice->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &depthDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE, // Estado inicial listo para escribir profundidad
            &optClear,
            IID_PPV_ARGS(&m_depthTexture)))) 
        {
            throw std::runtime_error("Fallo al crear el recurso de textura del Depth Buffer.");
        }

        // 4. Crear la vista (Descriptor) que conecta el Heap con la Textura
        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Texture2D.MipSlice = 0;

        nativeDevice->CreateDepthStencilView(m_depthTexture.Get(), &dsvDesc, GetView());
    }
}