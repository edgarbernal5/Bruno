#include "TextureProcessor.h"

#include "Pipeline/Graphics/TextureContentItem.h"

#include "Bruno/Core/Memory.h"
#include "Bruno/Platform/DirectX/D3DHelpers.h"
#include <filesystem>
#include <DirectXTex.h>

namespace Bruno
{
	BR_RTTI_DEFINITIONS(TextureProcessor);

	TextureProcessor::TextureProcessor()
	{
		m_supportedExtensions = { L".dds", L".bmp", L".jpg", L".png" };
	}

	std::shared_ptr<ContentItem> TextureProcessor::Process(const std::wstring& assetFilename, ContentProcessorContext& context)
	{
		std::filesystem::path filePath(assetFilename);
		DirectX::TexMetadata  metadata;
		DirectX::ScratchImage scratchImage;

		if (filePath.extension() == ".dds")
		{
			HRESULT hr = LoadFromDDSFile(assetFilename.c_str(), DirectX::DDS_FLAGS_FORCE_RGB, &metadata, scratchImage);
			if (FAILED(hr))
			{
			}
			ThrowIfFailed(hr);
		}
		else
		{
			HRESULT hr = LoadFromWICFile(assetFilename.c_str(), DirectX::WIC_FLAGS_FORCE_RGB, &metadata, scratchImage);
			if (FAILED(hr))
			{
			}
			ThrowIfFailed(hr);
		}

		auto outputContentItem = std::make_shared<TextureContentItem>();
		outputContentItem->Width = metadata.width;
		outputContentItem->Height = metadata.height;
		outputContentItem->Depth = metadata.depth;
		outputContentItem->ArraySize = metadata.arraySize;
		outputContentItem->MipLevels = metadata.mipLevels;
		outputContentItem->Format = metadata.format;
		outputContentItem->Dimension = metadata.dimension;

		DXGI_FORMAT textureFormat = metadata.format;
		bool is3DTexture = metadata.dimension == DirectX::TEX_DIMENSION_TEXTURE3D;

		//D3D12_RESOURCE_DESC resourceDesc{};
		//resourceDesc.Format = textureFormat;
		//resourceDesc.Width = metadata.width;
		//resourceDesc.Height = static_cast<uint32_t>(metadata.height);
		//resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		//resourceDesc.DepthOrArraySize = static_cast<uint16_t>(is3DTexture ? metadata.depth : metadata.arraySize);
		//resourceDesc.MipLevels = static_cast<uint16_t>(metadata.mipLevels);
		//resourceDesc.SampleDesc.Count = 1;
		//resourceDesc.SampleDesc.Quality = 0;
		//resourceDesc.Dimension = is3DTexture ? D3D12_RESOURCE_DIMENSION_TEXTURE3D : D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		//resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		//resourceDesc.Alignment = 0;

		size_t rowPitch, slicePitch;
		DirectX::ComputePitch(textureFormat, metadata.width, metadata.height, rowPitch, slicePitch);

		uint32_t subResourceCount = static_cast<uint32_t>(metadata.mipLevels * metadata.arraySize);
		
		//https://learn.microsoft.com/en-us/windows/win32/direct3d12/subresources
		//https://learn.microsoft.com/en-us/windows/win32/direct3d12/upload-and-readback-of-texture-data
		// 
		//int textureHeapSize = ((((width * numBytesPerPixel) + 255) & ~255) * (height - 1)) + (width * numBytesPerPixel);
		//https://www.braynzarsoft.net/viewtutorial/q16390-directx-12-textures-from-file
		outputContentItem->DataSizeInBytes = AlignU64(rowPitch, 256) * (metadata.height - 1) + (rowPitch);

		outputContentItem->Pixels.resize(outputContentItem->DataSizeInBytes);

		uint64_t offset = 0;
		const DirectX::Image* pImages = scratchImage.GetImages();
		//TODO: add support for 3d texture, mip mapping, cube maps.
		//See DetermineImageArray function in DirectXTexImage.cpp (DirectxTex) as an example.
		for (size_t i = 0; i < scratchImage.GetImageCount(); i++)
		{
			auto& imageContentItem = outputContentItem->Images.emplace_back();
			imageContentItem.RowPitch = pImages[i].rowPitch;
			imageContentItem.SlicePitch = pImages[i].slicePitch;
			imageContentItem.Offset = offset;
			memcpy(outputContentItem->Pixels.data() + offset, pImages[i].pixels, pImages[i].slicePitch);

			offset += pImages[i].slicePitch;
		}
		
		return outputContentItem;
	}
}