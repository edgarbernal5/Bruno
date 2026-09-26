#include "brpch.h"
#include "ColorBuffer.h"

#include "D3DFunctions.h"

namespace Bruno
{
    BR_RTTI_DEFINITIONS(ColorBuffer);
    
    ColorBuffer::ColorBuffer(GraphicsDevice& device, uint32_t width, uint32_t height, TextureFormat format, DescriptorAllocator& srvAllocator, DescriptorAllocator& rtvAllocator, const std::wstring& name)
    {
        auto nativeDevice = device.GetNativeDevice();
        DXGI_FORMAT dxgiFormat = D3DFunctions::GetDX12Format(format);
        
        m_width = width;
        m_height = height;
        m_format = format;
        m_mipMapLevels = 1;
        
        D3D12_RESOURCE_DESC texDesc = {};
        texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        texDesc.Width = width;
        texDesc.Height = height;
        texDesc.DepthOrArraySize = 1;
        texDesc.MipLevels = 1;
        texDesc.Format = dxgiFormat;
        texDesc.SampleDesc.Count = 1;
        texDesc.SampleDesc.Quality = 0;
        texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        
        // ¡CRÍTICO! Le decimos a DX12 que esta textura es un lienzo de escritura
        texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

        // 2. Definir el valor de limpieza optimizado (Clear Value)
        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = dxgiFormat;
        clearValue.Color[0] = 0.0f; // R
        clearValue.Color[1] = 0.0f; // G
        clearValue.Color[2] = 0.0f; // B
        clearValue.Color[3] = 0.0f; // A (0.0f es vital para el G-Buffer para evitar artefactos)

        auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        
        // Lo hacemos nacer en RENDER_TARGET state para que el Pase 1 escriba sin requerir barreras iniciales
        ThrowIfFailed(nativeDevice->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &texDesc,
            D3D12_RESOURCE_STATE_RENDER_TARGET, 
            &clearValue,
            IID_PPV_ARGS(&m_resource)
        ));

        // Nombrar el recurso para que PIX / RenderDoc muestren el nombre del G-Buffer
        m_resource->SetName(name.c_str());

        // 4. Crear el RTV (Render Target View)
        m_rtvAllocation = rtvAllocator.Allocate(1);
        
        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        rtvDesc.Format = dxgiFormat;
        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        
        nativeDevice->CreateRenderTargetView(m_resource.Get(), &rtvDesc, m_rtvAllocation.CPU);
        
        m_srvAllocation = srvAllocator.Allocate(1); 
        
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = dxgiFormat;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;

        nativeDevice->CreateShaderResourceView(m_resource.Get(), &srvDesc, m_srvAllocation.CPU);
    }

    void ColorBuffer::AttachNativeResource(const Microsoft::WRL::ComPtr<ID3D12Resource>& resource, const DescriptorAllocation& rtvAllocation)
    {
        m_resource = resource;
        m_rtvAllocation = rtvAllocation;
        
        // Opcional pero recomendado: Extraer el ancho y alto directamente del recurso nativo
        // auto desc = m_resource->GetDesc();
        // m_width = static_cast<uint32_t>(desc.Width);
        // m_height = desc.Height;
    }
}
