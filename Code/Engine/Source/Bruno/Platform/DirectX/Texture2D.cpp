#include "brpch.h"
#include "Texture2D.h"

#include "GraphicsDevice.h"
#include "UploadContext.h"

#include <DirectXTex.h> // Necesario para cargar texturas

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
            IID_PPV_ARGS(&m_textureResource)
        ));

        // 3. Subir los datos asíncronamente usando nuestro motor DMA
        D3D12_SUBRESOURCE_DATA subresourceData{};
        subresourceData.pData = image.GetPixels();
        subresourceData.RowPitch = image.GetImages()->rowPitch;
        subresourceData.SlicePitch = image.GetImages()->slicePitch;

        // ¡Una sola línea para toda la complejidad de subida!
        uploadContext.UploadTexture(m_textureResource.Get(), subresourceData);

        // 4. Crear el SRV (Shader Resource View)
        m_allocation = srvAllocator.Allocate(1); // O simplemente Allocate() según tu definición
    
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = metadata.format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;

        nativeDevice->CreateShaderResourceView(m_textureResource.Get(), &srvDesc, m_allocation.CPU);
    }
}