#pragma once

#include "D3DCommon.h"
#include "GPUBuffer.h"

namespace Bruno
{
	class IndexBuffer : public GPUBuffer
	{
	public:
		IndexBuffer(uint32_t numElements, const void* bufferData, uint32_t elementSize);
		~IndexBuffer() = default;
		
		uint32_t GetNumIndices() const { return m_numElements; }

		constexpr const D3D12_INDEX_BUFFER_VIEW& GetView() { return m_indexBufferView; }
	private:
		uint32_t m_numElements;
		uint32_t m_elementSize;

		D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
	};
}
