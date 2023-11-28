#pragma once

#include <d3d12.h>
#include "Bruno/Platform/DirectX/GpuBuffer.h"

namespace Bruno
{
	class GraphicsDevice;
	class GraphicsContext;

	class PrimitiveBatchBase
	{
	protected:
		PrimitiveBatchBase(GraphicsDevice* device, uint64_t maxIndices, uint64_t maxVertices, uint64_t vertexSize);
		virtual ~PrimitiveBatchBase();

	public:
		void Begin(GraphicsContext* context);
		void End();

	protected:
		void Draw(D3D_PRIMITIVE_TOPOLOGY topology, bool isIndexed, uint16_t const* indices, size_t indexCount, size_t vertexCount, void** pMappedVertices);
		void Flush();

		bool m_isIndexed;
		bool m_isBatching;
		bool m_inBeginEndPair;

		uint64_t m_maxIndices;
		uint64_t m_maxVertices;
		uint64_t m_vertexSize;

		uint64_t m_indexCount;
		uint64_t m_vertexCount;

		uint64_t m_baseIndex;
		uint64_t m_baseVertex;

		D3D_PRIMITIVE_TOPOLOGY m_topology;
		GraphicsContext* m_graphicsContext;

		std::unique_ptr<GpuBuffer> m_indexBuffer;
		std::unique_ptr<GpuBuffer> m_vertexBuffer;

	};

	template<typename TVertex>
	class PrimitiveBatch : public PrimitiveBatchBase
	{
		static constexpr size_t DefaultBatchSize = 4096;

	public:
		PrimitiveBatch(GraphicsDevice* device, uint64_t maxIndices, uint64_t maxVertices) :
			PrimitiveBatchBase(device, maxIndices, maxVertices, sizeof(TVertex))
		{
			BufferCreationDesc uploadIndexBufferDesc;
			uploadIndexBufferDesc.Size = AlignU32(m_maxIndices * sizeof(uint16_t), 16);
			uploadIndexBufferDesc.AccessFlags = BufferAccessFlags::HostWritable;

			m_indexBuffer = std::make_unique<GpuBuffer>(*device, uploadIndexBufferDesc);

			BufferCreationDesc uploadVertexBufferDesc;
			uploadVertexBufferDesc.Size = AlignU32(m_maxVertices * m_vertexSize, 256);
			uploadVertexBufferDesc.AccessFlags = BufferAccessFlags::HostWritable;

			m_vertexBuffer = std::make_unique<GpuBuffer>(*device, uploadVertexBufferDesc);
		}

		void Draw(D3D_PRIMITIVE_TOPOLOGY topology, TVertex const* vertices, size_t vertexCount)
		{
			void* mappedVertices;

			PrimitiveBatchBase::Draw(topology, false, nullptr, 0, vertexCount, &mappedVertices);

			memcpy(mappedVertices, vertices, vertexCount * m_vertexSize);
		}

		void DrawIndexed(D3D_PRIMITIVE_TOPOLOGY topology, uint16_t const* indices, size_t indexCount, TVertex const* vertices, size_t vertexCount)
		{
			void* mappedVertices;

			PrimitiveBatchBase::Draw(topology, true, indices, indexCount, vertexCount, &mappedVertices);

			memcpy(mappedVertices, vertices, vertexCount * m_vertexSize);
		}

		void DrawLine(TVertex const& v1, TVertex const& v2)
		{
			TVertex* mappedVertices;

			PrimitiveBatchBase::Draw(D3D_PRIMITIVE_TOPOLOGY_LINELIST, false, nullptr, 0, 2, reinterpret_cast<void**>(&mappedVertices));

			mappedVertices[0] = v1;
			mappedVertices[1] = v2;
		}

		void DrawTriangle(TVertex const& v1, TVertex const& v2, TVertex const& v3)
		{
			TVertex* mappedVertices;

			PrimitiveBatchBase::Draw(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, false, nullptr, 0, 3, reinterpret_cast<void**>(&mappedVertices));

			mappedVertices[0] = v1;
			mappedVertices[1] = v2;
			mappedVertices[2] = v3;
		}

		void DrawQuad(TVertex const& v1, TVertex const& v2, TVertex const& v3, TVertex const& v4)
		{
			static const uint16_t quadIndices[] = { 0, 1, 2, 0, 2, 3 };

			TVertex* mappedVertices;

			PrimitiveBatchBase::Draw(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, true, quadIndices, 6, 4, reinterpret_cast<void**>(&mappedVertices));

			mappedVertices[0] = v1;
			mappedVertices[1] = v2;
			mappedVertices[2] = v3;
			mappedVertices[3] = v4;
		}
	};
}
