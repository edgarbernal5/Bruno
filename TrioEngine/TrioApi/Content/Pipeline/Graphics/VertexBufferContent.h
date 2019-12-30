#pragma once

#include "TrioApiRequisites.h"

#include "Content/Pipeline/ContentItem.h"
#include "Memory/BitCopier.h"

namespace TrioEngine
{
	class VertexDeclarationContent;

	class TRIO_API_EXPORT VertexBufferContent : public ContentItem
	{
	public:
		VertexBufferContent();
		VertexBufferContent(uint32_t size);
		~VertexBufferContent();

		template <class T>
		void Write(int offset, int stride, const T* data, uint32_t size);

		inline uint32_t GetVertexDataSize()
		{
			return m_vertexSize;
		}
		
		inline uint8_t* GetVertexData()
		{
			return m_vertexData;
		}

		inline VertexDeclarationContent* GetVertexDeclaration()
		{
			return m_vertexDeclaration;
		}

		inline void SetVertexDeclaration(VertexDeclarationContent* decl)
		{
			m_vertexDeclaration = decl;
		}
	private:
		uint8_t* m_vertexData;
		uint32_t m_vertexSize;

		VertexDeclarationContent* m_vertexDeclaration;
	};


	template <class T>
	void VertexBufferContent::Write(int offset, int stride, const T* data, uint32_t size)
	{
		int sourceStride = sizeof(T);

		if (size > 0)
		{
			int num2 = (offset + (stride * (size - 1))) + sourceStride;
			if (num2 > m_vertexSize)
			{
				uint8_t* buffer = new uint8_t[num2];
				memcpy(buffer, m_vertexData, m_vertexSize);
				m_vertexSize = num2;

				delete[] m_vertexData;

				m_vertexData = buffer;
			}
			BitCopier::Copy<T, uint8_t>(data, 0, sourceStride, m_vertexData, offset, stride, sourceStride, size, size, m_vertexSize);
		}
	}
}