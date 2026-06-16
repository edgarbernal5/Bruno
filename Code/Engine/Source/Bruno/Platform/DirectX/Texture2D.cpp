#include "brpch.h"
#include "Texture2D.h"

#include <DirectXTex.h> // Necesario para cargar texturas

namespace Bruno::DX 
{
    BR_RTTI_DEFINITIONS(Texture2D);
    
    Texture2D::Texture2D(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, DescriptorAllocator& srvAllocator, const std::wstring& filePath)
    {
        // 1. Cargar la imagen desde disco
        DirectX::ScratchImage image;
        HRESULT hr = DirectX::LoadFromWICFile(filePath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);
        BR_ASSERT(SUCCEEDED(hr), "No se pudo cargar la textura!");

        const auto& metadata = image.GetMetadata();

        // 2. Crear el recurso en Default Heap (VRAM)
        D3D12_RESOURCE_DESC texDesc = {};
        texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        texDesc.Width = metadata.width;
        texDesc.Height = static_cast<UINT>(metadata.height);
        texDesc.DepthOrArraySize = 1;
        texDesc.MipLevels = 1;
        texDesc.Format = metadata.format;
        texDesc.SampleDesc.Count = 1;
        texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

        D3D12_HEAP_PROPERTIES heapProps = { D3D12_HEAP_TYPE_DEFAULT };;
        device->CreateCommittedResource(
            &heapProps, D3D12_HEAP_FLAG_NONE, &texDesc, 
            D3D12_RESOURCE_STATE_COPY_DEST, nullptr, 
            IID_PPV_ARGS(&m_textureResource)
        );

        // B. Crear el UploadHeap (usando la clase que diseñamos antes)
        // Calculamos el tamaño necesario para la copia
        UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_textureResource.Get(), 0, 1);
        m_uploadHeap = std::make_unique<UploadHeap>(device, uploadBufferSize);

        // C. Subir los datos al UploadHeap
        // DirectXTK nos facilita el mapeo de los píxeles al formato de la GPU
        D3D12_SUBRESOURCE_DATA subresourceData{};
        subresourceData.pData = image.GetPixels();
        subresourceData.RowPitch = image.GetImages()->rowPitch;
        subresourceData.SlicePitch = image.GetImages()->slicePitch;

        UpdateSubresources(cmdList, m_textureResource.Get(), m_uploadHeap->GetResource(), 
                           0, 0, 1, &subresourceData);

        // D. Barrera: De COPY_DEST a PIXEL_SHADER_RESOURCE (para que el shader pueda leerla)
        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_textureResource.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
        );
        cmdList->ResourceBarrier(1, &barrier);

        // E. Crear el SRV (Shader Resource View) usando tu DescriptorAllocator
        m_allocation = srvAllocator.Allocate(); // Pedimos el "ticket" al heap de descriptores
        
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = metadata.format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;

        device->CreateShaderResourceView(m_textureResource.Get(), &srvDesc, m_allocation.CPU);
    }
}