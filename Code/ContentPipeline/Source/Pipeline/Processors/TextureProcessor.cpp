#include "TextureProcessor.h"

#include "Pipeline/Graphics/TextureContentItem.h"

#include <Bruno/Platform/DirectX/D3DCommon.h>
#include <Bruno/Platform/DirectX/GraphicsDevice.h>
#include <filesystem>
#include <Bruno/Core/Memory.h>

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

		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Format = textureFormat;
		resourceDesc.Width = metadata.width;
		resourceDesc.Height = static_cast<uint32_t>(metadata.height);
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		resourceDesc.DepthOrArraySize = static_cast<uint16_t>(is3DTexture ? metadata.depth : metadata.arraySize);
		resourceDesc.MipLevels = static_cast<uint16_t>(metadata.mipLevels);
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Dimension = is3DTexture ? D3D12_RESOURCE_DIMENSION_TEXTURE3D : D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDesc.Alignment = 0;

		auto device = Graphics::GetDevice();

		uint32_t numRows[32];
		uint64_t rowSizesInBytes[32];
		std::array<D3D12_PLACED_SUBRESOURCE_FOOTPRINT, 32> subResourceLayouts{ 0 };

		uint32_t subResourceCount = static_cast<uint32_t>(metadata.mipLevels * metadata.arraySize);
		device->GetD3DDevice()->GetCopyableFootprints(&resourceDesc, 0, subResourceCount, 0, subResourceLayouts.data(), numRows, rowSizesInBytes, &outputContentItem->DataSizeInBytes);

		outputContentItem->Pixels.resize(outputContentItem->DataSizeInBytes);

		//int textureHeapSize = ((((width * numBytesPerPixel) + 255) & ~255) * (height - 1)) + (width * numBytesPerPixel);
		//https://www.braynzarsoft.net/viewtutorial/q16390-directx-12-textures-from-file

		for (uint64_t arrayIndex = 0; arrayIndex < metadata.arraySize; arrayIndex++)
		{
			for (uint64_t mipIndex = 0; mipIndex < metadata.mipLevels; mipIndex++)
			{
				const uint64_t subResourceIndex = mipIndex + (arrayIndex * metadata.mipLevels);

				const D3D12_PLACED_SUBRESOURCE_FOOTPRINT& subResourceLayout = subResourceLayouts[subResourceIndex];
				const uint64_t subResourceHeight = numRows[subResourceIndex];
				const uint64_t subResourcePitch = AlignU32(subResourceLayout.Footprint.RowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
				const uint64_t subResourceDepth = subResourceLayout.Footprint.Depth;
				uint8_t* destinationSubResourceMemory = outputContentItem->Pixels.data() + subResourceLayout.Offset;

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

		//const DirectX::Image* pImages = scratchImage.GetImages();
		//for (size_t i = 0; i < scratchImage.GetImageCount(); i++)
		//{
		//	auto& imageContentItem = outputContentItem->Images.emplace_back();
		//	imageContentItem.RowPitch = pImages[i].rowPitch;
		//	imageContentItem.SlicePitch = pImages[i].slicePitch;
		//	imageContentItem.Pixels.resize(pImages[i].slicePitch); //TODO: 3d textures.
		//	memcpy(imageContentItem.Pixels.data(), pImages[i].pixels, pImages[i].slicePitch);
		//}

		return outputContentItem;
	}
}