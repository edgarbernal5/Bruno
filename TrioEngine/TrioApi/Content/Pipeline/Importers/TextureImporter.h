#pragma once

#include "TrioApiRequisites.h"

#include "IContentImporter.h"

namespace TrioEngine
{
	class BitmapContent;
	class TextureContent;

	class TRIO_API_EXPORT TextureImporter : public IContentImporter
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

			BitmapContentAndInfo() : BitmapContent(nullptr), BytesPerPixel(0) {}
		};
		BitmapContent* CreateAndFillBitmapContent(ILinfo ilInfo);
		BitmapContentAndInfo CreateAppropriateTypeOfBitmapContent(ILinfo ilInfo, int width, int height);

		void GetTargetFormatAndBpp(SurfaceFormat targetFormat, int &localBpp, ILint &targetToIl);

		TextureContent* Import2DTexture(ILinfo ilImageInfo);
		TextureContent* Import3DTexture(ILinfo ilImageInfo);

	};
}