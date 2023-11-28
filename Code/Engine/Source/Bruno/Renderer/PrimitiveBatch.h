#pragma once

#include <d3d12.h>
#include "Bruno/Platform/DirectX/GPUBuffer.h"

namespace Bruno
{
	class GraphicsDevice;
	class GraphicsContext;
	class GPUBuffer;

	template<typename TVertex>
	class PrimitiveBatch
	{
		static constexpr size_t DefaultBatchSize = 4096;

	public:
		PrimitiveBatch(GraphicsDevice* device, uint64_t maxIndices, uint64_t maxVertices);

		void Begin(GraphicsContext* context);

		void Draw(D3D_PRIMITIVE_TOPOLOGY topology, TVertex const* vertices, size_t vertexCount)
		{
		}

		void DrawIndexed(D3D_PRIMITIVE_TOPOLOGY topology, uint16_t const* indices, size_t indexCount, TVertex const* vertices, size_t vertexCount)
		{
		}

		void DrawLine(TVertex const& v1, TVertex const& v2)
		{
			TVertex* mappedVertices;

			Draw(D3D_PRIMITIVE_TOPOLOGY_LINELIST, false, nullptr, 0, 2, reinterpret_cast<void**>(&mappedVertices));

			mappedVertices[0] = v1;
			mappedVertices[1] = v2;
		}

		void DrawTriangle(TVertex const& v1, TVertex const& v2, TVertex const& v3)
		{
			TVertex* mappedVertices;

			Draw(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, false, nullptr, 0, 3, reinterpret_cast<void**>(&mappedVertices));

			mappedVertices[0] = v1;
			mappedVertices[1] = v2;
			mappedVertices[2] = v3;
		}

		void DrawQuad(TVertex const& v1, TVertex const& v2, TVertex const& v3, TVertex const& v4)
		{
			static const uint16_t quadIndices[] = { 0, 1, 2, 0, 2, 3 };

			TVertex* mappedVertices;

			Draw(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, true, quadIndices, 6, 4, reinterpret_cast<void**>(&mappedVertices));

			mappedVertices[0] = v1;
			mappedVertices[1] = v2;
			mappedVertices[2] = v3;
			mappedVertices[3] = v4;
		}

		void End();

	private:
		void Draw(D3D_PRIMITIVE_TOPOLOGY topology, bool isIndexed, uint16_t const* indices, size_t indexCount, size_t vertexCount, void** pMappedVertices);
		void Flush();

		uint64_t m_maxIndices;
		uint64_t m_maxVertices;

		size_t m_indexCount;
		size_t m_vertexCount;

		size_t m_baseIndex;
		size_t m_baseVertex;

		D3D_PRIMITIVE_TOPOLOGY m_topology;
		bool m_isIndexed;
		GraphicsContext* m_context;
		bool m_isBatching;

		std::unique_ptr<GPUBuffer> m_indexBuffer;
		std::unique_ptr<GPUBuffer> m_vertexBuffer;
	};

	template<typename TVertex>
	inline PrimitiveBatch<TVertex>::PrimitiveBatch(GraphicsDevice* device, uint64_t maxIndices, uint64_t maxVertices) :
		m_context(nullptr),
		m_isBatching(false),
		m_isIndexed(false),
		m_maxIndices(maxIndices),
		m_maxVertices(maxVertices),
		m_indexCount(0),
		m_vertexCount(0),
		m_baseIndex(0),
		m_baseVertex(0)
	{
		BufferCreationDesc uploadIndexBufferDesc;
		uploadIndexBufferDesc.Size = AlignU32(m_maxIndices * sizeof(uint16_t), 16);
		uploadIndexBufferDesc.AccessFlags = BufferAccessFlags::HostWritable;

		m_indexBuffer = std::make_unique<GPUBuffer>(device, uploadIndexBufferDesc);

		BufferCreationDesc uploadVertexBufferDesc;
		uploadVertexBufferDesc.Size = AlignU32(m_maxVertices * sizeof(TVertex), 256);
		uploadVertexBufferDesc.AccessFlags = BufferAccessFlags::HostWritable;

		m_vertexBuffer = std::make_unique<GPUBuffer>(device, uploadVertexBufferDesc);
	}
}
