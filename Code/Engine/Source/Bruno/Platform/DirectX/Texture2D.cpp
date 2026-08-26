#include "brpch.h"
#include "Texture2D.h"

#include "GraphicsDevice.h"
#include "UploadContext.h"

#include <DirectXTex.h> // Necesario para cargar texturas

#include "D3DFunctions.h"

namespace Bruno 
{
    BR_RTTI_DEFINITIONS(Texture2D);
    
    Texture2D::Texture2D(GraphicsDevice& device, UploadContext& uploadContext, DescriptorAllocator& srvAllocator, const std::wstring& filename)
    {
        std::filesystem::path filePath(filename);
        if (!std::filesystem::exists(filePath))
        {
            return;
        }
        
        // 1. Cargar la imagen desde disco
        DirectX::ScratchImage image;
        HRESULT hr = 0;//DirectX::LoadFromWICFile(filePath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);
        
        if (filePath.extension() == ".dds")
        {
            hr = DirectX::LoadFromDDSFile(filename.c_str(), DirectX::DDS_FLAGS_FORCE_RGB, nullptr, image);
        }
        else
        {
            hr = DirectX::LoadFromWICFile(filename.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);
        }
        BR_ASSERT(SUCCEEDED(hr), "No se pudo cargar la textura!");

        const auto& metadata = image.GetMetadata();

        // 2. Crear el recurso en Default Heap (VRAM pura)
        D3D12_RESOURCE_DESC texDesc = {};
        texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        texDesc.Width = metadata.width;
        texDesc.Height = static_cast<UINT>(metadata.height);
        texDesc.DepthOrArraySize = 1;
        texDesc.MipLevels = 1;
        texDesc.Format = metadata.format;
        texDesc.SampleDesc.Count = 1;
        texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

        auto nativeDevice = device.GetNativeDevice();
        
        // ¡La magia de DX12! Nace en estado COMMON
        auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        ThrowIfFailed(nativeDevice->CreateCommittedResource(
            &heapProps, 
            D3D12_HEAP_FLAG_NONE, 
            &texDesc, 
            D3D12_RESOURCE_STATE_COMMON, 
            nullptr, 
            IID_PPV_ARGS(&m_resource)
        ));

        // 3. Subir los datos asíncronamente usando nuestro motor DMA
        D3D12_SUBRESOURCE_DATA subresourceData{};
        subresourceData.pData = image.GetPixels();
        subresourceData.RowPitch = image.GetImages()->rowPitch;
        subresourceData.SlicePitch = image.GetImages()->slicePitch;

        // ¡Una sola línea para toda la complejidad de subida!
        uploadContext.UploadTexture(m_resource.Get(), subresourceData);

        // 4. Crear el SRV (Shader Resource View)
        m_srvAllocation = srvAllocator.Allocate(1); // O simplemente Allocate() según tu definición
    
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = metadata.format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;

        nativeDevice->CreateShaderResourceView(m_resource.Get(), &srvDesc, m_srvAllocation.CPU);
    }

    Texture2D::Texture2D(GraphicsDevice& device, uint32_t width, uint32_t height, TextureFormat format, DescriptorAllocator& srvAllocator, DescriptorAllocator& rtvAllocator, const std::wstring& name)
    {
        auto nativeDevice = device.GetNativeDevice();
        DXGI_FORMAT dxgiFormat = D3DFunctions::GetDX12Format(format); // Traducimos el enum agnóstico

        // 1. Describir el recurso
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

        // 3. Crear el recurso en la VRAM (DEFAULT HEAP)
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

        // 5. Crear el SRV (Shader Resource View) en el Mega Heap (Bindless)
        m_srvAllocation = srvAllocator.Allocate(1); 
        
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = dxgiFormat;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;

        // Inyectamos el SRV directamente en tu mega-heap
        nativeDevice->CreateShaderResourceView(m_resource.Get(), &srvDesc, m_srvAllocation.CPU);
    }

    void Texture2D::AttachNativeResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource, const DescriptorAllocation& rtvAllocation)
    {
        m_resource = resource;
        m_rtvAllocation = rtvAllocation;
        
        // Opcional pero recomendado: Extraer el ancho y alto directamente del recurso nativo
        // auto desc = m_resource->GetDesc();
        // m_width = static_cast<uint32_t>(desc.Width);
        // m_height = desc.Height;
    }
}
