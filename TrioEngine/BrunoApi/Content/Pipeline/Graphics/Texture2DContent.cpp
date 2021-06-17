#include "stdafx.h"
#include "Texture2DContent.h"

namespace TrioEngine
{
	Texture2DContent::Texture2DContent() 
		: TextureContent(MipmapChainCollection(1))
	{
	}


	Texture2DContent::~Texture2DContent()
	{
	}
}