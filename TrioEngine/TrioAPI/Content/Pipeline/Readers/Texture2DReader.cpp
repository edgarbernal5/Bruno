#include "stdafx.h"
#include "Texture2DReader.h"

#include "Content/GraphicsContentHelper.h"
#include "Content/ContentReader.h"

#include "Graphics/Texture2D.h"

namespace TrioEngine
{
	Texture2DReader::Texture2DReader()
	{
	}


	Texture2DReader::~Texture2DReader()
	{
	}

	void* Texture2DReader::Read(ContentReader* input)
	{
		SurfaceFormat format = (SurfaceFormat)input->ReadInt32();
		uint32_t width = input->ReadUInt32();
		uint32_t height = input->ReadUInt32();
		uint32_t mipmaps = input->ReadUInt32();

		//TO-DO: Obtener el device mediante el ContentReader.
		Texture2D* texture = new Texture2D(GraphicsContentHelper::GraphicsDeviceFromContentReader(input), width, height, mipmaps, format);
		for (uint32_t i = 0; i < mipmaps; i++)
		{
			uint32_t size = input->ReadUInt32();
			std::vector<uint8_t> bytes = input->ReadByteBuffer(size);

			texture->SetData<uint8_t>(i, nullptr, bytes.data(), size, 0, size);
		}
		return texture;
	}
}