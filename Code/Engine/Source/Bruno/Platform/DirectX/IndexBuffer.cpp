#include "brpch.h"
#include "IndexBuffer.h"

#include "GraphicsDevice.h"
#include "Bruno/Core/Memory.h"

#include "D3D12MemAlloc.h"
#include "UploadContext.h"

namespace Bruno
{
    IndexBuffer::IndexBuffer(uint32_t sizeInBytes, const void* bufferData, uint32_t stride) :
		GPUBuffer(*Graphics::GetDevice(), BufferCreationDesc::Create(sizeInBytes, stride, BufferViewFlags::srv, BufferAccessFlags::gpuOnly)),
		//m_numElements(numElements),
		//m_elementSize(stride),
		m_indexBufferView{}
	{
		auto bufferUpload = std::make_unique<BufferUpload>();
		bufferUpload->mBuffer = this;
		bufferUpload->mBufferData = std::make_unique<uint8_t[]>(mDesc.Width);
		bufferUpload->mBufferDataSize = mDesc.Width;

		memcpy_s(bufferUpload->mBufferData.get(), mDesc.Width, bufferData, mDesc.Width);

		Graphics::GetDevice()->GetUploadContext().AddBufferUpload(std::move(bufferUpload));

		// Create the index buffer view.
		m_indexBufferView.BufferLocation = mResource->GetGPUVirtualAddress();
        m_indexBufferView.Format = stride == sizeof(uint16_t) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
        m_indexBufferView.SizeInBytes = static_cast<uint32_t>(mDesc.Width);
	}
}