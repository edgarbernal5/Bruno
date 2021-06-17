#include "stdafx.h"
#include "BitmapContent.h"

namespace TrioEngine
{
	BitmapContent::BitmapContent() 
		: ContentItem(), m_width(0), m_height(0)
	{
	}

	BitmapContent::BitmapContent(int width, int height) 
		: ContentItem(), m_width(width), m_height(height)
	{
	}

	BitmapContent::~BitmapContent()
	{
	}
}