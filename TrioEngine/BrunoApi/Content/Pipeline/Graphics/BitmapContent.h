#pragma once

#include "TrioApiRequisites.h"

#include "Content/Pipeline/ContentItem.h"

namespace BrunoEngine
{
	class BRUNO_API_EXPORT BitmapContent : public ContentItem
	{
	public:
		~BitmapContent();

		inline int GetHeight()
		{
			return m_height;
		}

		inline int GetWidth()
		{
			return m_width;
		}

		virtual uint8_t* GetPixelData(uint32_t &arrayLength) = 0;
		virtual void SetPixelData(uint8_t *dataRaw) = 0;
	protected:

		BitmapContent();
		BitmapContent(int width, int height);

		int m_width, m_height;
	};
}