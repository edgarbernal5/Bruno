#pragma once

#include "TrioApiRequisites.h"

#include "Content/Pipeline/ContentItem.h"
#include "MipmapChain.h"

namespace BrunoEngine
{
	class BRUNO_API_EXPORT TextureContent : public ContentItem
	{
	public:
		TextureContent(MipmapChainCollection faces);
		~TextureContent();

		void ConvertBitmapType();
		virtual void GenerateMipmaps(bool overwriteExistingMipmaps);

		const MipmapChainCollection& GetFaces();

	protected:
		MipmapChainCollection m_faces;
	};
}