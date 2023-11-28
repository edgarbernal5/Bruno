#include "brpch.h"
#include "PrimitiveBatch.h"

#include "Bruno/Platform/DirectX/GraphicsContext.h"

namespace Bruno
{
	template<typename TVertex>
	void PrimitiveBatch<TVertex>::Begin(GraphicsContext* context)
	{
		m_context = context;
	}

	template<typename TVertex>
	void PrimitiveBatch<TVertex>::End()
	{
	}

	template<typename TVertex>
	void PrimitiveBatch<TVertex>::Draw(D3D_PRIMITIVE_TOPOLOGY topology, bool isIndexed, uint16_t const* indices, size_t indexCount, size_t vertexCount, void** pMappedVertices)
	{
		BR_ASSERT(pMappedVertices != nullptr);

		const bool wrapIndexBuffer = (m_indexCount + indexCount > m_maxIndices);
		const bool wrapVertexBuffer = (m_vertexCount + vertexCount > m_maxVertices);

		if ((topology != m_topology) ||
			(isIndexed != m_isIndexed) ||
			wrapIndexBuffer || wrapVertexBuffer)
		{
			Flush();
		}

		if (!m_isBatching)
		{
			m_isBatching = true;
			m_indexCount = 0;
			m_vertexCount = 0;
			m_baseIndex = 0;
			m_baseVertex = 0;
			m_topology = topology;
			m_isIndexed = isIndexed;


		}
	}
	template<typename TVertex>
	void PrimitiveBatch<TVertex>::Flush()
	{

		m_isBatching = false;
	}
}