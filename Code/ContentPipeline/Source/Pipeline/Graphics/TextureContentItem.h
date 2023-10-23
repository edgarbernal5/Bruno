#pragma once

#include <vector>
#include "Pipeline/ContentItem.h"

namespace Bruno
{
	struct TextureContentItem : public ContentItem
	{
		BR_RTTI_DECLARATION(TextureContentItem, ContentItem);

	public:
		size_t Width;
		size_t Height;
		size_t Depth;
		size_t ArraySize;
		size_t MipLevels;
		uint32_t Format;
		uint8_t Dimension;
		uint64_t DataSizeInBytes;

		std::vector<uint8_t> Pixels;
		//struct ImageContentItem
		//{
		//	int64_t RowPitch;
		//	int64_t SlicePitch;
		//	std::vector<uint8_t> Pixels;
		//};
		//std::vector<ImageContentItem> Images;
	};

}