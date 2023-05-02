#pragma once

#include "D3DCommon.h"

namespace Bruno
{
	class IndexBuffer
	{
	public:
		IndexBuffer(uint32_t numElements, const void* bufferData, uint32_t elementSize);
		~IndexBuffer() = default;
		
		uint32_t GetNumIndices() const { return m_numElements; }

		const D3D12_INDEX_BUFFER_VIEW& GetView() { return m_indexBufferView; }
	private:
		uint32_t m_numElements;
		uint32_t m_elementSize;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_indexBuffer;
		D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
	};
}
