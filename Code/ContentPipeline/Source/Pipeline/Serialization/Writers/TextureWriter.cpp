#include "TextureWriter.h"

#include "Pipeline/Graphics/TextureContentItem.h"

namespace Bruno
{
	TextureWriter::TextureWriter() :
		AbstractContentTypeWriter(TextureContentItem::GetTypeIdClass())
	{
	}

	std::string TextureWriter::GetReaderName()
	{
		return "TextureReader";
	}

	void TextureWriter::Write(ContentWriter& output, const ContentItem& contentItem)
	{
		auto textueContentItem = contentItem.As<TextureContentItem>();
		output.WriteUInt64(textueContentItem->Width);
		output.WriteUInt64(textueContentItem->Height);
		output.WriteUInt64(textueContentItem->Depth);
		output.WriteUInt64(textueContentItem->ArraySize);
		output.WriteUInt64(textueContentItem->MipLevels);
		output.WriteUInt32(textueContentItem->Format);
		output.WriteUInt8(textueContentItem->Dimension);

		output.WriteUInt64(textueContentItem->DataSizeInBytes);
		output.WriteBytes(textueContentItem->Pixels);

		output.WriteUInt64(textueContentItem->Images.size());
		for (size_t i = 0; i < textueContentItem->Images.size(); i++)
		{
			output.WriteInt64(textueContentItem->Images[i].RowPitch);
			output.WriteInt64(textueContentItem->Images[i].SlicePitch);
			output.WriteUInt64(textueContentItem->Images[i].Offset);
			//output.WriteBytes(textueContentItem->Images[i].Pixels);
		}
	}
}
