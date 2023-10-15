#include "brpch.h"
#include "VertexBuffer.h"

#include "GraphicsDevice.h"
#include "UploadContext.h"

namespace Bruno
{
	VertexBuffer::VertexBuffer(uint32_t sizeInBytes, const void* bufferData, uint32_t vertexStride) :
		GPUBuffer(*Graphics::GetDevice(), BufferCreationDesc::Create(sizeInBytes, vertexStride, BufferViewFlags::srv, BufferAccessFlags::gpuOnly)),
		m_vertexBufferView{}
	{
		auto bufferUpload = std::make_unique<BufferUpload>();
		bufferUpload->mBuffer = this;
		bufferUpload->mBufferData = std::make_unique<uint8_t[]>(mDesc.Width);
		bufferUpload->mBufferDataSize = mDesc.Width;

		memcpy_s(bufferUpload->mBufferData.get(), mDesc.Width, bufferData, mDesc.Width);

		Graphics::GetDevice()->GetUploadContext().AddBufferUpload(std::move(bufferUpload));

		// Create the vertex buffer view.
		m_vertexBufferView.BufferLocation = mResource->GetGPUVirtualAddress();
        m_vertexBufferView.SizeInBytes = static_cast<uint32_t>(mDesc.Width);
        m_vertexBufferView.StrideInBytes = vertexStride;
	}
}