#include "brpch.h"
#include "Texture.h"

#include "GraphicsDevice.h"
#include "UploadCommand.h"

namespace Bruno
{
    Texture::Texture(const std::wstring& filename)
    {
        std::filesystem::path filePath(filename);
        if (!std::filesystem::exists(filePath))
        {
            return;
        }

        DirectX::TexMetadata  metadata;
        DirectX::ScratchImage scratchImage;

        if (filePath.extension() == ".dds")
        {
            ThrowIfFailed(LoadFromDDSFile(filename.c_str(), DirectX::DDS_FLAGS_FORCE_RGB, &metadata, scratchImage));
        }
        else
        {
            ThrowIfFailed(LoadFromWICFile(filename.c_str(), DirectX::WIC_FLAGS_FORCE_RGB, &metadata, scratchImage));
        }
        metadata.format = MakeSRGB(metadata.format);

        D3D12_RESOURCE_DESC textureDesc = {};
        switch (metadata.dimension)
        {
        case DirectX::TEX_DIMENSION_TEXTURE1D:
            textureDesc = CD3DX12_RESOURCE_DESC::Tex1D(metadata.format, static_cast<uint64_t>(metadata.width),
                static_cast<uint16_t>(metadata.arraySize));
            break;
        case DirectX::TEX_DIMENSION_TEXTURE2D:
            textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(metadata.format, static_cast<uint64_t>(metadata.width),
                static_cast<uint32_t>(metadata.height),
                static_cast<uint16_t>(metadata.arraySize));
            break;
        case DirectX::TEX_DIMENSION_TEXTURE3D:
            textureDesc = CD3DX12_RESOURCE_DESC::Tex3D(metadata.format, static_cast<uint64_t>(metadata.width),
                static_cast<uint32_t>(metadata.height),
                static_cast<uint16_t>(metadata.depth));
            break;
        default:
            throw std::exception("Invalid texture dimension.");
            break;
        }
        auto device = Graphics::GetDevice();
        
        ThrowIfFailed(device->GetD3DDevice()->CreateCommittedResource(
            &Graphics::Core::HeapProperties.DefaultHeap, 
            D3D12_HEAP_FLAG_NONE, 
            &textureDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&m_d3d12Resource)));

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = metadata.format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;// m_d3d12Resource->GetDesc().MipLevels;
        srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

        m_srvHandle = device->GetSrvDescriptionHeap().Allocate();
        device->GetD3DDevice()->CreateShaderResourceView(m_d3d12Resource.Get(), &srvDesc, m_srvHandle.Cpu);

        std::vector<D3D12_SUBRESOURCE_DATA> subresources(scratchImage.GetImageCount());
        const DirectX::Image* pImages = scratchImage.GetImages();
        for (int i = 0; i < scratchImage.GetImageCount(); ++i)
        {
            auto& subresource = subresources[i];
            subresource.RowPitch = pImages[i].rowPitch;
            subresource.SlicePitch = pImages[i].slicePitch;
            subresource.pData = pImages[i].pixels;
        }

        CopyTextureSubresource(0, static_cast<uint32_t>(subresources.size()), subresources.data());
        if (subresources.size() < m_d3d12Resource->GetDesc().MipLevels)
        {
            GenerateMips();
        }
    }

    void Texture::GenerateMips()
    {

    }

    DXGI_FORMAT Texture::MakeSRGB(DXGI_FORMAT fmt)
    {
        switch (fmt)
        {
        case DXGI_FORMAT_R8G8B8A8_UNORM:
            return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

        case DXGI_FORMAT_BC1_UNORM:
            return DXGI_FORMAT_BC1_UNORM_SRGB;

        case DXGI_FORMAT_BC2_UNORM:
            return DXGI_FORMAT_BC2_UNORM_SRGB;

        case DXGI_FORMAT_BC3_UNORM:
            return DXGI_FORMAT_BC3_UNORM_SRGB;

        case DXGI_FORMAT_B8G8R8A8_UNORM:
            return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

        case DXGI_FORMAT_B8G8R8X8_UNORM:
            return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;

        case DXGI_FORMAT_BC7_UNORM:
            return DXGI_FORMAT_BC7_UNORM_SRGB;

        default:
            return fmt;
        }
    }

    /*
    ver primal. d3d12_texture (D3D12Resources.cpp)
    */
	Texture::Texture(const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE* clearValue)
	{
        auto device = Graphics::GetDevice();
        if (clearValue)
        {
            m_d3d12ClearValue = std::make_unique<D3D12_CLEAR_VALUE>(*clearValue);
        }

        ThrowIfFailed(device->GetD3DDevice()->CreateCommittedResource(
            &Graphics::Core::HeapProperties.DefaultHeap,
            D3D12_HEAP_FLAG_NONE, 
            &resourceDesc,
            D3D12_RESOURCE_STATE_COMMON,
            m_d3d12ClearValue.get(), 
            IID_PPV_ARGS(&m_d3d12Resource)));

	}

    void Texture::CopyTextureSubresource(uint32_t firstSubresource, uint32_t numSubresources, D3D12_SUBRESOURCE_DATA* subresourceData)
    {
        if (m_d3d12Resource) {
            auto device = Graphics::GetDevice();

            uint64_t requiredSize = GetRequiredIntermediateSize(m_d3d12Resource.Get(), firstSubresource, numSubresources);

            auto uploadCommand = device->GetUploadCommand();
            uploadCommand->BeginUpload(requiredSize);
            uploadCommand->Update(m_d3d12Resource, subresourceData, firstSubresource, numSubresources);
            uploadCommand->EndUpload();

            /*

            // Create a temporary (intermediate) resource for uploading the subresources
            Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource;
            ThrowIfFailed(device->GetD3DDevice()->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Buffer(requiredSize),
                D3D12_RESOURCE_STATE_GENERIC_READ, 
                nullptr,
                IID_PPV_ARGS(&intermediateResource)));
*/
            //UpdateSubresources(device->GetCommandQueue()->GetCommandList(), m_d3d12Resource.Get(), intermediateResource.Get(), 0,
            //    firstSubresource, numSubresources, subresourceData);

        }
    }
}