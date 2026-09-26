#include "brpch.h"
#include "DepthBuffer.h"

#include "D3DFunctions.h"
#include "GraphicsDevice.h"
#include "Bruno/Renderer/Material.h"

namespace Bruno
{
    BR_RTTI_DEFINITIONS(DepthBuffer);
    
    DepthBuffer::DepthBuffer(GraphicsDevice& device, uint32_t width, uint32_t height, TextureFormat format) :
        m_device(device)
    {
        auto nativeDevice = m_device.GetNativeDevice();

        m_width = width;
        m_height = height;
        m_format = format;
        
        auto& dsvAllocator = m_device.GetDSVDescriptorAllocator();
        m_dsvHandle = dsvAllocator.Allocate(1);
        CreateResourceAndDescriptor();
    }

    DepthBuffer::~DepthBuffer() = default;

    void DepthBuffer::Resize(uint32_t width, uint32_t height)
    {
        if (m_width == width && m_height == height)
        {
            return;
        }
        
        m_width = width;
        m_height = height;
        
        // Liberar la textura actual antes de redimensionar
        m_resource.Reset();
        CreateResourceAndDescriptor();
    }

    void DepthBuffer::CreateResourceAndDescriptor()
    {
        auto nativeDevice = m_device.GetNativeDevice();

        auto d3dTextureFormat = D3DFunctions::GetDX12Format(m_format);
        
        // 2. Describir la memoria de la textura
        D3D12_RESOURCE_DESC depthDesc = {};
        depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        depthDesc.Width = m_width;
        depthDesc.Height = m_height;
        depthDesc.DepthOrArraySize = 1;
        depthDesc.MipLevels = 1;
        depthDesc.Format = d3dTextureFormat;
        depthDesc.SampleDesc.Count = 1;           // Sin Anti-Aliasing (MSAA) por ahora
        depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // ¡Vital para que funcione como Depth Buffer!

        // Optimizamos la limpieza (Clear) indicando el valor por defecto (1.0f = lo más lejano)
        D3D12_CLEAR_VALUE optClear = {};
        optClear.Format = d3dTextureFormat;
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
            IID_PPV_ARGS(&m_resource)))) 
        {
            throw std::runtime_error("Fallo al crear el recurso de textura del Depth Buffer.");
        }

        // 4. Crear la vista (Descriptor) que conecta el Heap con la Textura
        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Format = d3dTextureFormat;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Texture2D.MipSlice = 0;

        nativeDevice->CreateDepthStencilView(m_resource.Get(), &dsvDesc, m_dsvHandle.CPU);
    }
}
