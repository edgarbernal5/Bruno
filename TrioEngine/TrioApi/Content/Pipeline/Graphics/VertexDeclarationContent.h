#pragma once

#include "TrioApiRequisites.h"

#include "Content/Pipeline/ContentItem.h"

namespace TrioEngine
{
	class TRIO_API_EXPORT VertexDeclarationContent : public ContentItem
	{
	public:
		VertexDeclarationContent();

		inline std::vector<VertexElement>& GetVertexElements()
		{
			return m_vertexElements;
		}

		inline int* GetVertexStride()
		{
			if (m_bvertexStride)
				return &m_vertexStride;

			return nullptr;
		}

		inline void SetVertexStride(int* stride)
		{
			m_bvertexStride = (stride != nullptr);
			if (m_bvertexStride)
				m_vertexStride = *stride;
		}
	private:
		std::vector<VertexElement> m_vertexElements;
		int m_vertexStride;
		bool m_bvertexStride;
	};
}