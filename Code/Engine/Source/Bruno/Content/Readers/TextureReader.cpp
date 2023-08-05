#include "brpch.h"
#include "TextureReader.h"

#include "Bruno/Content/ContentReader.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(TextureReader);

	TextureReader::TextureReader() :
		ContentTypeReader(Texture::GetTypeIdClass())
	{
	}

	std::shared_ptr<Texture> TextureReader::ReadInternal(ContentReader& input)
	{
		Texture::InitData initData;
		input.ReadUInt64(initData.Width);
		input.ReadUInt64(initData.Height);
		input.ReadUInt64(initData.Depth);
		input.ReadUInt64(initData.ArraySize);
		input.ReadUInt64(initData.MipLevels);
		input.ReadUInt32(initData.Format);
		input.ReadUInt8(initData.Dimension);

		uint64_t imagesCount;
		input.ReadUInt64(imagesCount);

		for (size_t i = 0; i < imagesCount; i++)
		{
			auto& imageInitData = initData.Images.emplace_back();

			input.ReadInt64(imageInitData.RowPitch);
			input.ReadInt64(imageInitData.SlicePitch);
			input.ReadBytes(imageInitData.Pixels);
		}

		return std::make_shared<Texture>(initData);
	}
}