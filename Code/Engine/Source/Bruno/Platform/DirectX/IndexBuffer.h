#pragma once

#include "GPUBuffer.h"

namespace Bruno
{
	class GraphicsDevice;

	class IndexBuffer : public GPUBuffer
	{
	public:
		IndexBuffer(uint32_t sizeInBytes, const void* bufferData, uint32_t stride);
		~IndexBuffer() = default;
		
		constexpr const D3D12_INDEX_BUFFER_VIEW& GetView() { return m_indexBufferView; }
	private:
		D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
	};
}
