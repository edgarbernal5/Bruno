#include "stdafx.h"
#include "Texture2DWriter.h"

#include "ContentWriter.h"
#include "Content/Pipeline/Graphics/BitmapContent.h"
#include "Content/Pipeline/Graphics/Texture2DContent.h"

namespace TrioEngine
{
	Texture2DWriter::Texture2DWriter()
	{
	}

	Texture2DWriter::~Texture2DWriter()
	{
	}

	void Texture2DWriter::Write(ContentWriter *output, ContentItem* value)
	{
		Texture2DContent* content2D = static_cast<Texture2DContent*>(value);
		SurfaceFormat format = SurfaceFormat::Color;
		BitmapContent* content = (content2D->GetFaces()[0])[0];

		WriteTextureHeader(output, format, content->GetWidth(), content->GetHeight(), content2D->GetFaces().size(), content2D->GetFaces()[0].size());
		WriteTextureData(output, content2D);
	}

	void Texture2DWriter::WriteTextureHeader(ContentWriter* output, SurfaceFormat format, int width, int height, int depth, int mipLevels)
	{
		output->WriteInt32((int)format);
		output->WriteUInt32((uint32_t)width);
		output->WriteUInt32((uint32_t)height);
		output->WriteUInt32((uint32_t)mipLevels);
	}

	void Texture2DWriter::WriteTextureData(ContentWriter* output, Texture2DContent* texture)
	{
		for (int i = 0; i < texture->GetFaces().size(); i++)
		{
			const MipmapChain& chain = texture->GetFaces()[i];
			for (int j = 0; j < chain.size(); j++)
			{
				uint32_t length;
				uint8_t* pixelData = (chain)[0]->GetPixelData(length);
				output->WriteUInt32(length);
				output->WriteBytes(pixelData, length);
			}
		}
	}
}