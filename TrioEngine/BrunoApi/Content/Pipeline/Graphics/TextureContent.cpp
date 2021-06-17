#include "stdafx.h"
#include "TextureContent.h"

namespace BrunoEngine
{
	TextureContent::TextureContent(MipmapChainCollection faces) : m_faces(faces)
	{
	}

	TextureContent::~TextureContent()
	{
	}

	void TextureContent::GenerateMipmaps(bool overwriteExistingMipmaps)
	{
	}

	void TextureContent::ConvertBitmapType()
	{
		//foreach (MipmapChain chain in this.faces)
		for (int i = 0; i < m_faces.size(); i++)
		{
			for (int j = 0; j < m_faces[i].size(); j++)
			{
				BitmapContent *source = m_faces[i][j];
				//typeof(a);
				const char *pp = typeid(int).raw_name();
			}
		}
	}

	const MipmapChainCollection& TextureContent::GetFaces()
	{
		return m_faces;
	}
}