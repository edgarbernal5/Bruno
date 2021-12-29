#include "stdafx.h"
#include "Texture2DContent.h"

namespace BrunoEngine
{
	Texture2DContent::Texture2DContent() 
		: TextureContent(MipmapChainCollection(1))
	{
	}


	Texture2DContent::~Texture2DContent()
	{
	}
}