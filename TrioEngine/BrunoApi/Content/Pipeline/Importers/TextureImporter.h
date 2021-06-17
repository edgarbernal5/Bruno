#pragma once

#include "TrioApiRequisites.h"

#include "IContentImporter.h"

namespace BrunoEngine
{
	class BitmapContent;
	class TextureContent;

	class BRUNO_API_EXPORT TextureImporter : public IContentImporter
	{
	public:
		TextureImporter();
		~TextureImporter();

		ContentItem* Import(const std::string& filename);

		friend class BuildCoordinator;

		std::string GetImporterName() const { return "TextureImporter"; }
		std::string GetFileExtensions() const { return "jpg, png, bmp, dds, tiff, tga"; }

	private:
		struct BitmapContentAndInfo
		{
			BitmapContent* BitmapContent;
			int BytesPerPixel;
			ILint TargetFormatToDevIl;
			ILenum TargetTypeToDevIl;

			BitmapContentAndInfo() : BitmapContent(nullptr), BytesPerPixel(0), TargetFormatToDevIl(0), TargetTypeToDevIl(0) {}
		};
		BitmapContent* CreateAndFillBitmapContent(ILinfo& ilInfo);
		BitmapContentAndInfo CreateAppropriateTypeOfBitmapContent(ILinfo& ilInfo, int width, int height);

		void GetTargetFormatAndBpp(SurfaceFormat targetFormat, int &localBpp, ILint &targetToIl);

		TextureContent* Import2DTexture(ILinfo& ilImageInfo);
		TextureContent* Import3DTexture(ILinfo& ilImageInfo);

	};
}