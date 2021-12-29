#pragma once

#include "TrioApiRequisites.h"

#include <string>
#include <vector>

namespace BrunoEngine
{
	class BRUNO_API_EXPORT TextureLoader
	{
	public:
		TextureLoader();
		~TextureLoader();

		enum class TextureLoaderType
		{
			UnsignedByte = IL_UNSIGNED_BYTE,
			Short = IL_SHORT,
			UnsignedShort = IL_UNSIGNED_SHORT,
			Int = IL_INT,
			UnsignedInt = IL_UNSIGNED_INT,
			Float = IL_FLOAT,
			Double = IL_DOUBLE,
			Half = IL_HALF
		};

		enum class TextureLoaderFormat
		{
			COLOR_INDEX = IL_COLOR_INDEX,
			ALPHA = IL_ALPHA,
			RGB = IL_RGB,
			RGBA = IL_RGBA,
			BGR = IL_BGR,
			BGRA = IL_BGRA,
			LUMINANCE = IL_LUMINANCE,
			LUMINANCE_ALPHA = IL_LUMINANCE_ALPHA
		};

		struct ImageInfo
		{
			uint8_t *Data;      // the image's data
			uint32_t  Width;      // the image's width
			uint32_t  Height;     // the image's height
			uint32_t  Depth;      // the image's depth
			uint8_t Bpp;        // bytes per pixel (not bits) of the image
			uint32_t  SizeOfData; // the total size of the data (in bytes)
			TextureLoaderFormat  Format;     // image format (in IL enum style)
			TextureLoaderType  Type;       // image type (in IL enum style)
			
			ILuint  NumMips;    // number of mipmaps

			ImageInfo() : 
				Data(nullptr),
				Width(0),
				Height(0),
				Depth(0),
				Bpp(0),
				SizeOfData(0)

			{
				Format = TextureLoaderFormat::RGB;
				Type = TextureLoaderType::UnsignedByte;
			}
		};

		ImageInfo GetTextureFromFile(std::string csFilename, TextureLoaderType targetType = TextureLoaderType::UnsignedByte, SurfaceFormat targetFormat = SurfaceFormat::Color);
	private:
		ILuint m_imgId;
		std::vector<uint8_t*> m_buffers;
	};
}