#pragma once

#include "TrioApiRequisites.h"

#include "BitmapContent.h"

namespace TrioEngine
{
	template <class T>
	class TRIO_API_EXPORT PixelBitmapContent : public BitmapContent
	{
	public:
		PixelBitmapContent(int width, int height);
		~PixelBitmapContent();
		 
		T& GetPixel(int x, int y);
		T* GetRow(int y);
		void SetPixel(int x, int y, T& value);
		
		uint8_t* GetPixelData(uint32_t &arrayLength);
		void SetPixelData(uint8_t *dataRaw);
	protected:
		PixelBitmapContent();

		T* m_pixelData;
		uint32_t m_sizeData;
	};
	
	template <class T>
	PixelBitmapContent<T>::PixelBitmapContent() 
		: BitmapContent(), m_pixelData(nullptr), m_sizeData(0)
	{
	}
	
	template <class T>
	PixelBitmapContent<T>::PixelBitmapContent(int width, int height) 
		: BitmapContent(width, height), m_pixelData(nullptr)
	{
		m_sizeData = width * height;
		m_pixelData = new T[m_sizeData];
	}

	template <class T>
	PixelBitmapContent<T>::~PixelBitmapContent()
	{
		if (m_pixelData != nullptr)
		{
			delete[] m_pixelData;
			m_pixelData = nullptr;
		}
	}
	
	template <class T>
	T& PixelBitmapContent<T>::GetPixel(int x, int y)
	{
		return m_pixelData[y * m_width + x];
	}

	template <class T>
	T* PixelBitmapContent<T>::GetRow(int y)
	{
		return m_pixelData[y * m_width];
	}

	template <class T>
	void PixelBitmapContent<T>::SetPixel(int x, int y, T& value)
	{
		m_pixelData[y * m_width + x] = value;
	}

	template <class T>
	void PixelBitmapContent<T>::SetPixelData(uint8_t *dataRaw)
	{
		memcpy(m_pixelData, dataRaw, sizeof(T) * m_sizeData);
	}
	
	template <class T>
	uint8_t* PixelBitmapContent<T>::GetPixelData(uint32_t &arrayLength)
	{
		UINT32 pixelSize = sizeof(T);
		arrayLength = m_height * m_width * pixelSize;
		return (uint8_t*)(&m_pixelData[0]);
	}
}

