#pragma once

#include "GpuBuffer.h"

namespace Bruno
{
	class GraphicsDevice;

	class IndexBuffer : public GpuBuffer
	{
	public:
		IndexBuffer(uint32_t sizeInBytes, const void* bufferData, uint32_t stride);
		~IndexBuffer() = default;
		
		constexpr const D3D12_INDEX_BUFFER_VIEW& GetView() { return m_indexBufferView; }
	private:
		D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
	};
}
