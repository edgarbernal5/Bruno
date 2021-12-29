#include "stdafx.h"
#include "TextureLoader.h"


namespace BrunoEngine
{
	TextureLoader::TextureLoader() : 
		m_imgId(0)
	{
		ilInit();
		iluInit();
	}

	TextureLoader::~TextureLoader()
	{
		for (size_t i = 0; i < m_buffers.size(); i++)
		{
			delete[] m_buffers[i];
		}
		m_buffers.clear();
	}

	TextureLoader::ImageInfo TextureLoader::GetTextureFromFile(std::string csFilename, TextureLoaderType targetType, SurfaceFormat targetFormat)
	{
		// Generate the main image name to use.
		ilGenImages(1, &m_imgId);

		// Bind this image name.
		ilBindImage(m_imgId);

		wchar_t bufferFilename[256];
		int nChars = MultiByteToWideChar(CP_ACP, 0, csFilename.c_str(), -1, NULL, 0);
		MultiByteToWideChar(CP_ACP, 0, csFilename.c_str(), -1, bufferFilename, nChars);

		if (!ilLoadImage(bufferFilename))
		{
			return TextureLoader::ImageInfo();
		}

		ILinfo ilInfo;
		iluGetImageInfo(&ilInfo);

		TextureLoader::ImageInfo imageInfo;
		imageInfo.Width = ilInfo.Width;
		imageInfo.Height = ilInfo.Width;
		imageInfo.Bpp = ilInfo.Bpp;
		imageInfo.NumMips = ilInfo.NumMips;
		imageInfo.Type = (TextureLoaderType)ilInfo.Type;
		imageInfo.Format = (TextureLoaderFormat)ilInfo.Format;

		ILint targetToIl = 0;
		int localBpp = 0;
		switch (targetFormat)
		{
		case BrunoEngine::SurfaceFormat::ColorSRgb:
		case BrunoEngine::SurfaceFormat::Color:
			targetToIl = IL_RGBA;
			localBpp = 4;
			break;
		case BrunoEngine::SurfaceFormat::Bgr32:
		case BrunoEngine::SurfaceFormat::Bgr32SRgb:
		case BrunoEngine::SurfaceFormat::Bgr565:
			targetToIl = IL_BGR;
			localBpp = 3;
			break;
		case BrunoEngine::SurfaceFormat::Bgra32:
		case BrunoEngine::SurfaceFormat::Bgra4444:
		case BrunoEngine::SurfaceFormat::Bgra32SRgb:
		case BrunoEngine::SurfaceFormat::Bgra5551:
			targetToIl = IL_BGRA;
			localBpp = 4;
			break;

		case BrunoEngine::SurfaceFormat::Alpha8:
			targetToIl = IL_ALPHA;
			localBpp = 1;
			break;
		default:
			targetToIl = IL_RGBA;
			localBpp = 4;
			break;
		}
		uint32_t bufferSize = ilInfo.Width * ilInfo.Height * ilInfo.Depth * localBpp;
		ILubyte	*buffer = new ILubyte[bufferSize];
		if (buffer == nullptr)
		{
			return TextureLoader::ImageInfo();
		}

		imageInfo.Data = buffer;
		m_buffers.push_back(buffer);

		ilCopyPixels(0, 0, 0, ilInfo.Width, ilInfo.Height, ilInfo.Depth, targetToIl, (ILenum)targetType, buffer);

		imageInfo.SizeOfData = bufferSize;

		ilDeleteImages(1, &m_imgId);

		return imageInfo;
	}
}