#pragma once

#include "D3DCommon.h"

namespace Bruno
{
	class VertexBuffer
	{
	public:
		VertexBuffer(uint32_t numVertices, const void* bufferData, uint32_t vertexStride);
		~VertexBuffer() = default;

	private:
		uint32_t m_numVertices;
		uint32_t m_vertexStride;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	};
}
