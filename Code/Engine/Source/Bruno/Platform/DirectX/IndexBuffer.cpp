#include "brpch.h"
#include "IndexBuffer.h"

#include "GraphicsDevice.h"
#include "Bruno/Core/Memory.h"

#include "D3D12MemAlloc.h"
#include "UploadContext.h"

namespace Bruno
{
    IndexBuffer::IndexBuffer(uint32_t sizeInBytes, const void* bufferData, uint32_t stride) :
		GPUBuffer(*Graphics::GetDevice(), BufferCreationDesc::Create(sizeInBytes, stride, BufferViewFlags::Srv, BufferAccessFlags::GpuOnly)),
		m_indexBufferView{}
	{
		auto bufferUpload = std::make_unique<BufferUpload>();
		bufferUpload->Buffer = this;
		bufferUpload->BufferData = std::make_unique<uint8_t[]>(m_desc.Width);
		bufferUpload->BufferDataSize = m_desc.Width;

		memcpy_s(bufferUpload->BufferData.get(), m_desc.Width, bufferData, m_desc.Width);

		Graphics::GetDevice()->GetUploadContext().AddBufferUpload(std::move(bufferUpload));

		m_indexBufferView.BufferLocation = m_resource->GetGPUVirtualAddress();
        m_indexBufferView.Format = stride == sizeof(uint16_t) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
        m_indexBufferView.SizeInBytes = static_cast<uint32_t>(m_desc.Width);
	}
}