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
    }

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
            ThrowIfFailed(LoadFromDDSFile(filename.c_str(), DirectX::DDS_FLAGS_NONE, &metadata, scratchImage));
        }
        else
        {
            ThrowIfFailed(LoadFromWICFile(filename.c_str(), DirectX::WIC_FLAGS_NONE, &metadata, scratchImage));
        }
        //metadata.format = MakeSRGB(metadata.format);

        DXGI_FORMAT textureFormat = metadata.format;
        bool is3DTexture = metadata.dimension == DirectX::TEX_DIMENSION_TEXTURE3D;

        TextureCreationDesc desc;
        desc.mResourceDesc.Format = textureFormat;
        desc.mResourceDesc.Width = metadata.width;
        desc.mResourceDesc.Height = static_cast<uint32_t>(metadata.height);
        desc.mResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
        desc.mResourceDesc.DepthOrArraySize = static_cast<uint16_t>(is3DTexture ? metadata.depth : metadata.arraySize);
        desc.mResourceDesc.MipLevels = static_cast<uint16_t>(metadata.mipLevels);
        desc.mResourceDesc.SampleDesc.Count = 1;
        desc.mResourceDesc.SampleDesc.Quality = 0;
        desc.mResourceDesc.Dimension = is3DTexture ? D3D12_RESOURCE_DIMENSION_TEXTURE3D : D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.mResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        desc.mResourceDesc.Alignment = 0;
        desc.mViewFlags = TextureViewFlags::Srv;

        auto device = Graphics::GetDevice();

        //CreateTexture
        CreateTexture(desc);
        auto textureUpload = std::make_unique<TextureUpload>();

        UINT numRows[Graphics::Core::MAX_TEXTURE_SUBRESOURCE_COUNT];
        uint64_t rowSizesInBytes[Graphics::Core::MAX_TEXTURE_SUBRESOURCE_COUNT];

        textureUpload->mTexture = this;
        textureUpload->mNumSubResources = static_cast<uint32_t>(metadata.mipLevels * metadata.arraySize);

        device->GetD3DDevice()->GetCopyableFootprints(&desc.mResourceDesc, 0, textureUpload->mNumSubResources, 0, textureUpload->mSubResourceLayouts.data(), numRows, rowSizesInBytes, &textureUpload->mTextureDataSize);

        textureUpload->mTextureData = std::make_unique<uint8_t[]>(textureUpload->mTextureDataSize);

        for (uint64_t arrayIndex = 0; arrayIndex < metadata.arraySize; arrayIndex++)
        {
            for (uint64_t mipIndex = 0; mipIndex < metadata.mipLevels; mipIndex++)
            {
                const uint64_t subResourceIndex = mipIndex + (arrayIndex * metadata.mipLevels);

                const D3D12_PLACED_SUBRESOURCE_FOOTPRINT& subResourceLayout = textureUpload->mSubResourceLayouts[subResourceIndex];
                const uint64_t subResourceHeight = numRows[subResourceIndex];
                const uint64_t subResourcePitch = AlignU32(subResourceLayout.Footprint.RowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
                const uint64_t subResourceDepth = subResourceLayout.Footprint.Depth;
                uint8_t* destinationSubResourceMemory = textureUpload->mTextureData.get() + subResourceLayout.Offset;

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

        //mSRVDescriptor = device->GetSrvDescriptionHeap().Allocate();
        //device->GetD3DDevice()->CreateShaderResourceView(m_d3d12Resource.Get(), &srvDesc, mSRVDescriptor.Cpu);

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
	Texture::Texture(const InitData& textureInitData)
	{
        auto device = Graphics::GetDevice();

        D3D12_RESOURCE_DESC textureDesc = {};
        switch ((DirectX::TEX_DIMENSION)textureInitData.Dimension)
        {
        case DirectX::TEX_DIMENSION_TEXTURE1D:
            textureDesc = CD3DX12_RESOURCE_DESC::Tex1D((DXGI_FORMAT)textureInitData.Format, static_cast<uint64_t>(textureInitData.Width),
                static_cast<uint16_t>(textureInitData.ArraySize));
            break;
        case DirectX::TEX_DIMENSION_TEXTURE2D:
            textureDesc = CD3DX12_RESOURCE_DESC::Tex2D((DXGI_FORMAT)textureInitData.Format, static_cast<uint64_t>(textureInitData.Width),
                static_cast<uint32_t>(textureInitData.Height),
                static_cast<uint16_t>(textureInitData.ArraySize));
            break;
        case DirectX::TEX_DIMENSION_TEXTURE3D:
            textureDesc = CD3DX12_RESOURCE_DESC::Tex3D((DXGI_FORMAT)textureInitData.Format, static_cast<uint64_t>(textureInitData.Width),
                static_cast<uint32_t>(textureInitData.Height),
                static_cast<uint16_t>(textureInitData.Depth));
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
            IID_PPV_ARGS(&mResource)));

        auto desk = mResource->GetDesc();
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = (DXGI_FORMAT)textureInitData.Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;// m_d3d12Resource->GetDesc().MipLevels;
        srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
        
        mSRVDescriptor = device->GetSrvDescriptionHeap().Allocate();
        device->GetD3DDevice()->CreateShaderResourceView(mResource, &srvDesc, mSRVDescriptor.Cpu);

        std::vector<D3D12_SUBRESOURCE_DATA> subresources(textureInitData.Images.size());
        
        for (int i = 0; i < textureInitData.Images.size(); ++i)
        {
            auto& subresource = subresources[i];
            subresource.RowPitch = textureInitData.Images[i].RowPitch;
            subresource.SlicePitch = textureInitData.Images[i].SlicePitch;
            subresource.pData = textureInitData.Images[i].Pixels.data();
        }

        CopyTextureSubresource(0, static_cast<uint32_t>(subresources.size()), subresources.data());
        if (subresources.size() < mResource->GetDesc().MipLevels)
        {
            GenerateMips();
        }
    }

    Texture::~Texture()
    {
        auto device = Graphics::GetDevice();
        device->GetSrvDescriptionHeap().Free(mSRVDescriptor);
        
    }

    void Texture::CreateTexture(TextureCreationDesc& desc)
    {
        auto device = Graphics::GetDevice();

        D3D12_RESOURCE_DESC textureDesc = desc.mResourceDesc;
        textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        bool hasRTV = ((desc.mViewFlags & TextureViewFlags::Rtv) == TextureViewFlags::Rtv);
        bool hasDSV = ((desc.mViewFlags & TextureViewFlags::Dsv) == TextureViewFlags::Dsv);
        bool hasSRV = ((desc.mViewFlags & TextureViewFlags::Srv) == TextureViewFlags::Srv);
        bool hasUAV = ((desc.mViewFlags & TextureViewFlags::Uav) == TextureViewFlags::Uav);

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
            switch (desc.mResourceDesc.Format)
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
                BR_ASSERT(false, "Bad depth stencil format.");
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

        mDesc = textureDesc;
        mState = resourceState;

        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = desc.mResourceDesc.Format;

        if (hasDSV)
        {
            clearValue.DepthStencil.Depth = 1.0f;
        }

        D3D12MA::ALLOCATION_DESC allocationDesc{};
        allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

        device->GetAllocator()->CreateResource(&allocationDesc, &textureDesc, resourceState, (!hasRTV && !hasDSV) ? nullptr : &clearValue, &this->mAllocation, IID_PPV_ARGS(&this->mResource));

        if (hasSRV)
        {
            mSRVDescriptor = device->GetSrvDescriptionHeap().Allocate();
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

                device->GetD3DDevice()->CreateShaderResourceView(mResource, &srvDesc, mSRVDescriptor.Cpu);
            }
            else
            {
                D3D12_SHADER_RESOURCE_VIEW_DESC* srvDescPointer = nullptr;
                D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
                bool isCubeMap = desc.mResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D && desc.mResourceDesc.DepthOrArraySize == 6;

                if (isCubeMap)
                {
                    shaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
                    shaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                    shaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
                    shaderResourceViewDesc.TextureCube.MipLevels = desc.mResourceDesc.MipLevels;
                    shaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
                    srvDescPointer = &shaderResourceViewDesc;
                }

                device->GetD3DDevice()->CreateShaderResourceView(mResource, srvDescPointer, mSRVDescriptor.Cpu);
            }

            mDescriptorHeapIndex = device->GetFreeReservedDescriptorIndex();

            device->CopySRVHandleToReservedTable(mSRVDescriptor, mDescriptorHeapIndex);
        }

        if (hasRTV)
        {
            mRTVDescriptor = device->GetRtvDescriptionHeap().Allocate();
            device->GetD3DDevice()->CreateRenderTargetView(mResource, nullptr, mRTVDescriptor.Cpu);
        }

        if (hasDSV)
        {
            D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
            dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
            dsvDesc.Format = desc.mResourceDesc.Format;
            dsvDesc.Texture2D.MipSlice = 0;
            dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

            mDSVDescriptor = device->GetDsvDescriptionHeap().Allocate();
            device->GetD3DDevice()->CreateDepthStencilView(mResource, &dsvDesc, mDSVDescriptor.Cpu);
        }

        if (hasUAV)
        {
            mUAVDescriptor = device->GetSrvDescriptionHeap().Allocate();
            device->GetD3DDevice()->CreateUnorderedAccessView(mResource, nullptr, nullptr, mUAVDescriptor.Cpu);
        }

        mIsReady = (hasRTV || hasDSV);

       
    }

    void Texture::CopyTextureSubresource(uint32_t firstSubresource, uint32_t numSubresources, D3D12_SUBRESOURCE_DATA* subresourceData)
    {
        if (mResource) {
//            auto device = Graphics::GetDevice();
//
//            uint64_t requiredSize = GetRequiredIntermediateSize(mResource, firstSubresource, numSubresources);
//
//            auto uploadCommand = device->GetUploadCommand();
//            uploadCommand->BeginUpload(requiredSize);
//            uploadCommand->Update(mResource, subresourceData, firstSubresource, numSubresources);
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