#include "brpch.h"
#include "VertexBuffer.h"

#include "GraphicsDevice.h"

namespace Bruno
{
	VertexBuffer::VertexBuffer(uint32_t sizeInBytes, const void* bufferData, uint32_t vertexStride) :
		GPUBuffer(*Graphics::GetDevice(), BufferCreationDesc::Create(sizeInBytes, vertexStride, BufferViewFlags::srv, BufferAccessFlags::gpuOnly)),
		m_vertexBufferView{}
	{
		// Create the vertex buffer view.
		m_vertexBufferView.BufferLocation = mResource->GetGPUVirtualAddress();
        m_vertexBufferView.SizeInBytes = static_cast<uint32_t>(mDesc.Width);
        m_vertexBufferView.StrideInBytes = vertexStride;
	}
}