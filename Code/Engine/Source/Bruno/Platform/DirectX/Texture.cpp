#include "brpch.h"
#include "Texture.h"

#include "GraphicsDevice.h"
#include "UploadContext.h"

#include "D3D12MemAlloc.h"
#include "Bruno/Core/Memory.h"

namespace Bruno
{
    BR_RTTI_DEFINITIONS(Texture);

    Texture::Texture()
    {
        m_resourceType = GPUResourceType::Texture;
    }

    Texture::Texture(const std::wstring& filename)
    {
        m_resourceType = GPUResourceType::Texture;

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
            ThrowIfFailed(LoadFromWICFile(filename.c_str(), DirectX::WIC_FLAGS_NONE, &metadata, scratchImage));
        }

        DXGI_FORMAT textureFormat = metadata.format;
        bool is3DTexture = metadata.dimension == DirectX::TEX_DIMENSION_TEXTURE3D;

        TextureCreationDesc textureCreationdesc;
        textureCreationdesc.ResourceDesc.Format = textureFormat;
        textureCreationdesc.ResourceDesc.Width = metadata.width;
        textureCreationdesc.ResourceDesc.Height = static_cast<uint32_t>(metadata.height);
        textureCreationdesc.ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
        textureCreationdesc.ResourceDesc.DepthOrArraySize = static_cast<uint16_t>(is3DTexture ? metadata.depth : metadata.arraySize);
        textureCreationdesc.ResourceDesc.MipLevels = static_cast<uint16_t>(metadata.mipLevels);
        textureCreationdesc.ResourceDesc.SampleDesc.Count = 1;
        textureCreationdesc.ResourceDesc.SampleDesc.Quality = 0;
        textureCreationdesc.ResourceDesc.Dimension = is3DTexture ? D3D12_RESOURCE_DIMENSION_TEXTURE3D : D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        textureCreationdesc.ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        textureCreationdesc.ResourceDesc.Alignment = 0;
        textureCreationdesc.ViewFlags = TextureViewFlags::Srv;

        auto device = Graphics::GetDevice();

        CreateTexture(textureCreationdesc);
        auto textureUpload = std::make_unique<TextureUpload>();

        uint32_t numRows[Graphics::Core::MAX_TEXTURE_SUBRESOURCE_COUNT];
        uint64_t rowSizesInBytes[Graphics::Core::MAX_TEXTURE_SUBRESOURCE_COUNT];

        textureUpload->Texture = this;
        textureUpload->SubResourcesCount = static_cast<uint32_t>(metadata.mipLevels * metadata.arraySize);

        device->GetD3DDevice()->GetCopyableFootprints(&textureCreationdesc.ResourceDesc, 0, textureUpload->SubResourcesCount, 0, textureUpload->SubResourceLayouts.data(), numRows, rowSizesInBytes, &textureUpload->TextureDataSize);

        textureUpload->TextureData = std::make_unique<uint8_t[]>(textureUpload->TextureDataSize);

        for (uint64_t arrayIndex = 0; arrayIndex < metadata.arraySize; arrayIndex++)
        {
            for (uint64_t mipIndex = 0; mipIndex < metadata.mipLevels; mipIndex++)
            {
                const uint64_t subResourceIndex = mipIndex + (arrayIndex * metadata.mipLevels);

                const D3D12_PLACED_SUBRESOURCE_FOOTPRINT& subResourceLayout = textureUpload->SubResourceLayouts[subResourceIndex];
                const uint64_t subResourceHeight = numRows[subResourceIndex];
                const uint64_t subResourcePitch = AlignU32(subResourceLayout.Footprint.RowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
                const uint64_t subResourceDepth = subResourceLayout.Footprint.Depth;
                uint8_t* destinationSubResourceMemory = textureUpload->TextureData.get() + subResourceLayout.Offset;

                for (uint64_t sliceIndex = 0; sliceIndex < subResourceDepth; sliceIndex++)
                {
                    const DirectX::Image* subImage = scratchImage.GetImage(mipIndex, arrayIndex, sliceIndex);
                    const uint8_t* sourceSubResourceMemory = subImage->pixels;

                    for (uint64_t height = 0; height < subResourceHeight; height++)
                    {
                        memcpy(destinationSubResourceMemory, sourceSubResourceMemory, (std::min)(subResourcePitch, subImage->rowPitch));
                        destinationSubResourceMemory += subResourcePitch;
                        sourceSubResourceMemory += subImage->rowPitch;
                    }
                }
            }
        }

        device->GetUploadContext().AddTextureUpload(std::move(textureUpload));
        
        //ThrowIfFailed(device->GetD3DDevice()->CreateCommittedResource(
        //    &Graphics::Core::HeapProperties.DefaultHeap, 
        //    D3D12_HEAP_FLAG_NONE, 
        //    &textureDesc,
        //    D3D12_RESOURCE_STATE_COMMON,
        //    nullptr,
        //    IID_PPV_ARGS(&m_d3d12Resource)));

        //D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        //srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        //srvDesc.Format = metadata.format;
        //srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        //srvDesc.Texture2D.MostDetailedMip = 0;
        //srvDesc.Texture2D.MipLevels = 1;// m_d3d12Resource->GetDesc().MipLevels;
        //srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

        //m_srvDescriptor = device->GetSrvDescriptionHeap().Allocate();
        //device->GetD3DDevice()->CreateShaderResourceView(m_d3d12Resource.Get(), &srvDesc, m_srvDescriptor.Cpu);

        //std::vector<D3D12_SUBRESOURCE_DATA> subresources(scratchImage.GetImageCount());
        //const DirectX::Image* pImages = scratchImage.GetImages();
        //for (int i = 0; i < scratchImage.GetImageCount(); ++i)
        //{
        //    auto& subresource = subresources[i];
        //    subresource.RowPitch = pImages[i].rowPitch;
        //    subresource.SlicePitch = pImages[i].slicePitch;
        //    subresource.pData = pImages[i].pixels;
        //}

        //CopyTextureSubresource(0, static_cast<uint32_t>(subresources.size()), subresources.data());
        //if (subresources.size() < m_d3d12Resource->GetDesc().MipLevels)
        //{
        //    GenerateMips();
        //}
    }

    void Texture::GenerateMips()
    {
    }

	Texture::Texture(const AssetPipelineInitData& assetPipelineInitData)
	{
        m_resourceType = GPUResourceType::Texture;

        auto device = Graphics::GetDevice();

        D3D12_RESOURCE_DESC textureDesc = {};
        switch ((DirectX::TEX_DIMENSION)assetPipelineInitData.Dimension)
        {
        case DirectX::TEX_DIMENSION_TEXTURE1D:
            textureDesc = CD3DX12_RESOURCE_DESC::Tex1D((DXGI_FORMAT)assetPipelineInitData.Format, static_cast<uint64_t>(assetPipelineInitData.Width),
                static_cast<uint16_t>(assetPipelineInitData.ArraySize));
            break;
        case DirectX::TEX_DIMENSION_TEXTURE2D:
            textureDesc = CD3DX12_RESOURCE_DESC::Tex2D((DXGI_FORMAT)assetPipelineInitData.Format, static_cast<uint64_t>(assetPipelineInitData.Width),
                static_cast<uint32_t>(assetPipelineInitData.Height),
                static_cast<uint16_t>(assetPipelineInitData.ArraySize));
            break;
        case DirectX::TEX_DIMENSION_TEXTURE3D:
            textureDesc = CD3DX12_RESOURCE_DESC::Tex3D((DXGI_FORMAT)assetPipelineInitData.Format, static_cast<uint64_t>(assetPipelineInitData.Width),
                static_cast<uint32_t>(assetPipelineInitData.Height),
                static_cast<uint16_t>(assetPipelineInitData.Depth));
            break;
        default:
            throw std::exception("Invalid texture dimension.");
            break;
        }

        ThrowIfFailed(device->GetD3DDevice()->CreateCommittedResource(
            &Graphics::Core::HeapProperties.DefaultHeap,
            D3D12_HEAP_FLAG_NONE,
            &textureDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&m_resource)));

        auto desk = m_resource->GetDesc();
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = (DXGI_FORMAT)assetPipelineInitData.Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;// m_d3d12Resource->GetDesc().MipLevels;
        srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
        
        m_srvDescriptor = device->GetSrvDescriptionHeap().Allocate();
        device->GetD3DDevice()->CreateShaderResourceView(m_resource, &srvDesc, m_srvDescriptor.Cpu);

        std::vector<D3D12_SUBRESOURCE_DATA> subresources(assetPipelineInitData.Images.size());
        
        for (int i = 0; i < assetPipelineInitData.Images.size(); ++i)
        {
            auto& subresource = subresources[i];
            subresource.RowPitch = assetPipelineInitData.Images[i].RowPitch;
            subresource.SlicePitch = assetPipelineInitData.Images[i].SlicePitch;
            subresource.pData = assetPipelineInitData.Images[i].Pixels.data();
        }

        CopyTextureSubresource(0, static_cast<uint32_t>(subresources.size()), subresources.data());
        if (subresources.size() < m_resource->GetDesc().MipLevels)
        {
            GenerateMips();
        }
    }

    Texture::Texture(const TextureCreationDesc& textureDesc)
    {
        m_resourceType = GPUResourceType::Texture;

        CreateTexture(textureDesc);
    }

    Texture::~Texture()
    {
        auto device = Graphics::GetDevice();
        //TODO: add a new request to device to destroy the resource. we shouldn't do this here.

        if (m_rtvDescriptor.IsValid())
            device->GetRtvDescriptionHeap().Free(m_rtvDescriptor);

        if (m_srvDescriptor.IsValid())
            device->GetSrvDescriptionHeap().Free(m_srvDescriptor);

        if (m_dsvDescriptor.IsValid())
            device->GetDsvDescriptionHeap().Free(m_dsvDescriptor);
    }

    void Texture::CreateTexture(const TextureCreationDesc& textureCreationdesc)
    {
        auto device = Graphics::GetDevice();

        D3D12_RESOURCE_DESC textureDesc = textureCreationdesc.ResourceDesc;
        textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        bool hasRTV = ((textureCreationdesc.ViewFlags & TextureViewFlags::Rtv) == TextureViewFlags::Rtv);
        bool hasDSV = ((textureCreationdesc.ViewFlags & TextureViewFlags::Dsv) == TextureViewFlags::Dsv);
        bool hasSRV = ((textureCreationdesc.ViewFlags & TextureViewFlags::Srv) == TextureViewFlags::Srv);
        bool hasUAV = ((textureCreationdesc.ViewFlags & TextureViewFlags::Uav) == TextureViewFlags::Uav);

        D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COPY_DEST;
        DXGI_FORMAT resourceFormat = textureDesc.Format;
        DXGI_FORMAT shaderResourceViewFormat = textureDesc.Format;

        if (hasRTV)
        {
            textureDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
            resourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;
        }

        if (hasDSV)
        {
            switch (textureCreationdesc.ResourceDesc.Format)
            {
            case DXGI_FORMAT_D16_UNORM:
                resourceFormat = DXGI_FORMAT_R16_TYPELESS;
                shaderResourceViewFormat = DXGI_FORMAT_R16_UNORM;
                break;
            case DXGI_FORMAT_D24_UNORM_S8_UINT:
                resourceFormat = DXGI_FORMAT_R24G8_TYPELESS;
                shaderResourceViewFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
                break;
            case DXGI_FORMAT_D32_FLOAT:
                resourceFormat = DXGI_FORMAT_R32_TYPELESS;
                shaderResourceViewFormat = DXGI_FORMAT_R32_FLOAT;
                break;
            case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
                resourceFormat = DXGI_FORMAT_R32G8X24_TYPELESS;
                shaderResourceViewFormat = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
                break;
            default:
                BR_ASSERT_ERROR("Bad depth stencil format.");
                break;
            }

            textureDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
            resourceState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
        }

        if (hasUAV)
        {
            textureDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
            resourceState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        }

        textureDesc.Format = resourceFormat;

        m_desc = textureDesc;
        m_state = resourceState;

        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = textureCreationdesc.ResourceDesc.Format;

        if (hasDSV)
        {
            clearValue.DepthStencil.Depth = 1.0f;
        }

        D3D12MA::ALLOCATION_DESC allocationDesc{};
        allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

        device->GetAllocator()->CreateResource(&allocationDesc, &textureDesc, resourceState, (!hasRTV && !hasDSV) ? nullptr : &clearValue, &this->m_allocation, IID_PPV_ARGS(&this->m_resource));

        if (hasSRV)
        {
            m_srvDescriptor = device->GetSrvDescriptionHeap().Allocate();
            if (hasDSV)
            {
                D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
                srvDesc.Format = shaderResourceViewFormat;
                srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                srvDesc.Texture2D.MipLevels = 1;
                srvDesc.Texture2D.MostDetailedMip = 0;
                srvDesc.Texture2D.PlaneSlice = 0;
                srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
                srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;

                device->GetD3DDevice()->CreateShaderResourceView(m_resource, &srvDesc, m_srvDescriptor.Cpu);
            }
            else
            {
                D3D12_SHADER_RESOURCE_VIEW_DESC* srvDescPointer = nullptr;
                D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
                bool isCubeMap = textureCreationdesc.ResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D && textureCreationdesc.ResourceDesc.DepthOrArraySize == 6;

                if (isCubeMap)
                {
                    shaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
                    shaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                    shaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
                    shaderResourceViewDesc.TextureCube.MipLevels = textureCreationdesc.ResourceDesc.MipLevels;
                    shaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
                    srvDescPointer = &shaderResourceViewDesc;
                }

                device->GetD3DDevice()->CreateShaderResourceView(m_resource, srvDescPointer, m_srvDescriptor.Cpu);
            }

            m_descriptorHeapIndex = device->GetFreeReservedDescriptorIndex();

            device->CopySRVHandleToReservedTable(m_srvDescriptor, m_descriptorHeapIndex);
        }

        if (hasRTV)
        {
            m_rtvDescriptor = device->GetRtvDescriptionHeap().Allocate();
            device->GetD3DDevice()->CreateRenderTargetView(m_resource, nullptr, m_rtvDescriptor.Cpu);
        }

        if (hasDSV)
        {
            D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
            dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
            dsvDesc.Format = textureCreationdesc.ResourceDesc.Format;
            dsvDesc.Texture2D.MipSlice = 0;
            dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

            m_dsvDescriptor = device->GetDsvDescriptionHeap().Allocate();
            device->GetD3DDevice()->CreateDepthStencilView(m_resource, &dsvDesc, m_dsvDescriptor.Cpu);
        }

        if (hasUAV)
        {
            m_uavDescriptor = device->GetSrvDescriptionHeap().Allocate();
            device->GetD3DDevice()->CreateUnorderedAccessView(m_resource, nullptr, nullptr, m_uavDescriptor.Cpu);
        }

        m_isReady = (hasRTV || hasDSV);
    }

    void Texture::CopyTextureSubresource(uint32_t firstSubresource, uint32_t numSubresources, D3D12_SUBRESOURCE_DATA* subresourceData)
    {
        if (m_resource) {
//            auto device = Graphics::GetDevice();
//
//            uint64_t requiredSize = GetRequiredIntermediateSize(m_resource, firstSubresource, numSubresources);
//
//            auto uploadCommand = device->GetUploadCommand();
//            uploadCommand->BeginUpload(requiredSize);
//            uploadCommand->Update(m_resource, subresourceData, firstSubresource, numSubresources);
//            uploadCommand->EndUpload();
//
//            /*
//
//            // Create a temporary (intermediate) resource for uploading the subresources
//            Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource;
//            ThrowIfFailed(device->GetD3DDevice()->CreateCommittedResource(
//                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//                D3D12_HEAP_FLAG_NONE,
//                &CD3DX12_RESOURCE_DESC::Buffer(requiredSize),
//                D3D12_RESOURCE_STATE_GENERIC_READ,
//                nullptr,
//                IID_PPV_ARGS(&intermediateResource)));
//*/
////UpdateSubresources(device->GetCommandQueue()->GetCommandList(), m_d3d12Resource.Get(), intermediateResource.Get(), 0,
////    firstSubresource, numSubresources, subresourceData);

        }
    }
    /*
    ver primal. d3d12_texture (D3D12Resources.cpp)
    */
}