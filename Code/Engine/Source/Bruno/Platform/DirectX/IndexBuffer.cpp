#include "brpch.h"
#include "IndexBuffer.h"

#include "GraphicsDevice.h"
#include "UploadCommand.h"
#include "Bruno/Core/Memory.h"

#include "D3D12MemAlloc.h"

namespace Bruno
{
    IndexBuffer::IndexBuffer(uint32_t sizeInBytes, const void* bufferData, uint32_t stride) :
		GPUBuffer(*Graphics::GetDevice(), BufferCreationDesc::Create(sizeInBytes, stride, BufferViewFlags::srv, BufferAccessFlags::hostWritable)),
		//m_numElements(numElements),
		//m_elementSize(stride),
		m_indexBufferView{}
	{
		//if (isHostVisible)
		{
			SetMappedData(bufferData, sizeInBytes);
		}

		// Create the index buffer view.
		m_indexBufferView.BufferLocation = mResource->GetGPUVirtualAddress();
        m_indexBufferView.Format = stride == sizeof(uint16_t) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
        m_indexBufferView.SizeInBytes = static_cast<uint32_t>(mDesc.Width);
	}
}