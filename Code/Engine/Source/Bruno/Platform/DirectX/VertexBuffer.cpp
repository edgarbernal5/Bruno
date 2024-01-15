#include "brpch.h"
#include "VertexBuffer.h"

#include "GraphicsDevice.h"
#include "UploadContext.h"

namespace Bruno
{
	VertexBuffer::VertexBuffer(uint32_t sizeInBytes, const void* bufferData, uint32_t vertexStride) :
		GpuBuffer(*Graphics::GetDevice(), BufferCreationDesc::Create(sizeInBytes, vertexStride, BufferViewFlags::Srv, BufferAccessFlags::GpuOnly)),
		m_vertexBufferView{}
	{
		if (bufferData)
		{
			auto bufferUpload = std::make_unique<BufferUpload>();
			bufferUpload->Buffer = this;
			bufferUpload->BufferData = std::make_unique<uint8_t[]>(m_desc.Width);
			bufferUpload->BufferDataSize = m_desc.Width;

			memcpy_s(bufferUpload->BufferData.get(), m_desc.Width, bufferData, m_desc.Width);

			Graphics::GetDevice()->GetUploadContext().AddBufferUpload(std::move(bufferUpload));
		}

		m_vertexBufferView.BufferLocation = m_resource->GetGPUVirtualAddress();
        m_vertexBufferView.SizeInBytes = static_cast<uint32_t>(m_desc.Width);
        m_vertexBufferView.StrideInBytes = vertexStride;
	}
}