#include "stdafx.h"
#include "TextureImporter.h"

#include "Content/Pipeline/Graphics/PixelBitmapContent.h"
#include "Content/Pipeline/Graphics/Texture2DContent.h"

#include "Errors/ContentPipelineException.h"

namespace BrunoEngine
{
	TextureImporter::TextureImporter()
	{
		ilInit();
		iluInit();

		if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION ||
			iluGetInteger(ILU_VERSION_NUM) < ILU_VERSION) {
			//ilutGetInteger(ILUT_VERSION_NUM) < ILUT_VERSION) {
			printf("DevIL version is different...exiting!\n");
		}
	}

	TextureImporter::~TextureImporter()
	{
	}

	ContentItem* TextureImporter::Import(const std::string& filename)
	{
		ILuint imgId;

		// Generate the main image name to use.
		ilGenImages(1, &imgId);

		// Bind this image name.
		ilBindImage(imgId);

		wchar_t bufferFilename[256];
		int nChars = MultiByteToWideChar(CP_ACP, 0, filename.c_str(), -1, NULL, 0); //TODO
		MultiByteToWideChar(CP_ACP, 0, filename.c_str(), -1, bufferFilename, nChars);

		if (!ilLoadImage(bufferFilename))
		{
			return nullptr;
		}

		ILinfo ilInfo;
		iluGetImageInfo(&ilInfo);

		TextureContent *content = nullptr;
		if (ilInfo.Depth == 1)//CubeFlags
		{
			content = Import2DTexture(ilInfo);
		}

		ilDeleteImages(1, &imgId);
		return content;
	}

	BitmapContent* TextureImporter::CreateAndFillBitmapContent(ILinfo& ilInfo)
	{
		BitmapContentAndInfo item = CreateAppropriateTypeOfBitmapContent(ilInfo, ilInfo.Width, ilInfo.Height);

		uint32_t sizeInBytes = ilInfo.Width * ilInfo.Height * ilInfo.Depth * item.BytesPerPixel;
		ILubyte	*buffer = new ILubyte[sizeInBytes];

		//No estoy seguro si usar siempre en type = IL_UNSIGNED_BYTE, o segun tipo de formato
		ilCopyPixels(0, 0, 0, ilInfo.Width, ilInfo.Height, ilInfo.Depth, item.TargetFormatToDevIl, item.TargetTypeToDevIl, buffer);

		BitmapContent *bitmapContent = item.BitmapContent;
		bitmapContent->SetPixelData(buffer);

		delete[] buffer;

		return bitmapContent;
	}

	TextureImporter::BitmapContentAndInfo TextureImporter::CreateAppropriateTypeOfBitmapContent(ILinfo& ilInfo, int width, int height)
	{
		BitmapContentAndInfo info;

		switch (ilInfo.Type)
		{
		case IL_BYTE:
			break;
		case IL_UNSIGNED_BYTE:
			info.TargetTypeToDevIl = IL_UNSIGNED_BYTE;
			switch (ilInfo.Format)
			{
			case IL_RGB:
			case IL_BGR:
				info.BitmapContent = new PixelBitmapContent<ColorRGBA8>(ilInfo.Width, ilInfo.Height);
				info.BytesPerPixel = sizeof(ColorRGBA8);
				info.TargetFormatToDevIl = IL_RGBA;
				break;

			case IL_RGBA:
			case IL_BGRA:
				info.BitmapContent = new PixelBitmapContent<ColorRGBA8>(ilInfo.Width, ilInfo.Height);
				info.BytesPerPixel = sizeof(ColorRGBA8);
				info.TargetFormatToDevIl = IL_RGBA;
				break;
			default:
				break;
			}
			break;
		default:
			throw ContentPipelineException("ILinfo.Type is not supported");
			break;
		}

		return info;
	}

	void TextureImporter::GetTargetFormatAndBpp(SurfaceFormat targetFormat, int &localBpp, ILint &targetToIl)
	{
		targetToIl = 0;
		localBpp = 0;
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
	}

	TextureContent* TextureImporter::Import2DTexture(ILinfo& ilImageInfo)
	{
		Texture2DContent *content = new Texture2DContent();

		BitmapContent *item = CreateAndFillBitmapContent(ilImageInfo);

		content->GetMipmaps().push_back(item);
		return content;
	}
	
	TextureContent* TextureImporter::Import3DTexture(ILinfo& ilImageInfo)
	{
		return nullptr;
	}
}