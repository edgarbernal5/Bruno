#include "TextureProcessor.h"

#include "Pipeline/Graphics/TextureContentItem.h"

#include <Bruno/Platform/DirectX/D3DCommon.h>
#include <filesystem>

namespace Bruno
{
	BR_RTTI_DEFINITIONS(TextureProcessor);

	TextureProcessor::TextureProcessor()
	{
		m_supportedExtensions = { L".dds", L".bmp", L".jpg" };
	}

	std::shared_ptr<ContentItem> TextureProcessor::Process(const std::wstring& assetFilename)
	{
		std::filesystem::path filePath(assetFilename);
		DirectX::TexMetadata  metadata;
		DirectX::ScratchImage scratchImage;

		if (filePath.extension() == ".dds")
		{
			ThrowIfFailed(LoadFromDDSFile(assetFilename.c_str(), DirectX::DDS_FLAGS_FORCE_RGB, &metadata, scratchImage));
		}
		else
		{
			ThrowIfFailed(LoadFromWICFile(assetFilename.c_str(), DirectX::WIC_FLAGS_FORCE_RGB, &metadata, scratchImage));
		}

		auto outputContentItem = std::make_shared<TextureContentItem>();
		outputContentItem->Width = metadata.width;
		outputContentItem->Height = metadata.height;
		outputContentItem->Depth = metadata.depth;
		outputContentItem->ArraySize = metadata.arraySize;
		outputContentItem->MipLevels = metadata.mipLevels;
		outputContentItem->Format = metadata.format;
		outputContentItem->Dimension = metadata.dimension;

		const DirectX::Image* pImages = scratchImage.GetImages();
		for (size_t i = 0; i < scratchImage.GetImageCount(); i++)
		{
			auto& imageContentItem = outputContentItem->Images.emplace_back();
			imageContentItem.RowPitch = pImages[i].rowPitch;
			imageContentItem.SlicePitch = pImages[i].slicePitch;
			imageContentItem.Pixels.resize(pImages[i].slicePitch); //TODO: 3d textures.
			memcpy(imageContentItem.Pixels.data(), pImages[i].pixels, pImages[i].slicePitch);
		}
		return outputContentItem;
	}
}