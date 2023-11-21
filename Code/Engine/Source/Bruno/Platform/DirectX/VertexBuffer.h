#pragma once

#include "GPUBuffer.h"

namespace Bruno
{
	class VertexBuffer : public GPUBuffer
	{
	public:
		VertexBuffer(uint32_t numVertices, const void* bufferData, uint32_t vertexStride);
		~VertexBuffer() = default;

		constexpr const D3D12_VERTEX_BUFFER_VIEW& GetView() { return m_vertexBufferView; }
	private:

		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	};
}
