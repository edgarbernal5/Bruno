#pragma once

#include "TrioApiRequisites.h"

#include "TextureContent.h"

namespace TrioEngine
{
	class BRUNO_API_EXPORT Texture2DContent : public TextureContent
	{
	public:
		Texture2DContent();
		~Texture2DContent();

		inline MipmapChain& GetMipmaps()
		{
			return m_faces[0];
		}
	};
}