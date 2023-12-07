#include "brpch.h"
#include "PrimitiveBatch.h"

#include "Bruno/Platform/DirectX/GraphicsContext.h"

namespace Bruno
{
	PrimitiveBatchBase::PrimitiveBatchBase(GraphicsDevice* device, uint64_t maxIndices, uint64_t maxVertices, uint64_t vertexSize) :
		m_graphicsContext(nullptr),
		m_isBatching(false),
		m_isIndexed(false),
		m_inBeginEndPair(false),
		m_maxIndices(maxIndices),
		m_maxVertices(maxVertices),
		m_vertexSize(vertexSize),
		m_indexCount(0),
		m_vertexCount(0),
		m_baseIndex(0),
		m_baseVertex(0),
		m_topology(D3D_PRIMITIVE_TOPOLOGY_UNDEFINED)
	{
	}

	PrimitiveBatchBase::~PrimitiveBatchBase()
	{
	}

	void PrimitiveBatchBase::Begin(GraphicsContext* context)
	{
		if (m_inBeginEndPair)
			return;

		m_inBeginEndPair = true;
		m_graphicsContext = context;
	}

	void PrimitiveBatchBase::Draw(D3D_PRIMITIVE_TOPOLOGY topology, bool isIndexed, uint16_t const* indices, size_t indexCount, size_t vertexCount, void** pMappedVertices)
	{
		BR_ASSERT(pMappedVertices != nullptr);
		BR_ASSERT(indexCount < m_maxIndices, "Too many indices");
		BR_ASSERT(vertexCount < m_maxVertices, "Too many vertices");

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

		if (isIndexed)
		{
			auto outputIndices = reinterpret_cast<uint16_t*>(m_indexBuffer->GetMappedResource()) + m_indexCount;

			for (size_t i = 0; i < indexCount; i++)
			{
				outputIndices[i] = static_cast<uint16_t>(indices[i] + m_vertexCount - m_baseIndex);
			}

			m_indexCount += indexCount;
		}
		*pMappedVertices = reinterpret_cast<uint8_t*>(m_vertexBuffer->GetMappedResource()) + m_vertexSize * m_vertexCount;

		m_vertexCount += vertexCount;
	}

	void PrimitiveBatchBase::End()
	{
		if (!m_inBeginEndPair)
			return;

		Flush();
		m_inBeginEndPair = false;
		m_vertexBuffer->Reset();
		m_indexBuffer->Reset();
	}

	void PrimitiveBatchBase::Flush()
	{
		if (!m_isBatching)
			return;

		m_graphicsContext->SetPrimitiveTopology(m_topology);

		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = m_vertexBuffer->GetGpuAddress();
		vbv.SizeInBytes = static_cast<uint32_t>(m_vertexSize * (m_vertexCount - m_baseVertex));
		vbv.StrideInBytes = static_cast<uint32_t>(m_vertexSize);
		m_graphicsContext->SetVertexBuffer(vbv);

		if (m_isIndexed)
		{
			D3D12_INDEX_BUFFER_VIEW ibv;
			ibv.BufferLocation = m_indexBuffer->GetGpuAddress();
			ibv.Format = DXGI_FORMAT_R16_UINT;
			ibv.SizeInBytes = static_cast<uint32_t>(m_indexCount - m_baseIndex) * sizeof(uint16_t);
			m_graphicsContext->SetIndexBuffer(ibv);

			m_graphicsContext->DrawIndexedInstanced(static_cast<uint32_t>(m_indexCount - m_baseIndex), 1, 0, 0, 0);
		}
		else
		{
			m_graphicsContext->DrawInstanced(static_cast<uint32_t>(m_vertexCount - m_baseVertex), 1, 0, 0);
		}

		m_isBatching = false;
	}
}