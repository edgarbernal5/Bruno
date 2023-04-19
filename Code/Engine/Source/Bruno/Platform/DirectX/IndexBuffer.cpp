#include "brpch.h"
#include "IndexBuffer.h"

#include "GraphicsDevice.h"
#include "UploadCommand.h"

namespace Bruno
{

    IndexBuffer::IndexBuffer(uint32_t numElements, const void* bufferData, uint32_t elementSize) :
		m_numElements(numElements),
		m_elementSize(elementSize),
		m_indexBufferView{}
	{
		GraphicsDevice* device = Graphics::GetDevice();
		auto uploadCommand = device->GetUploadCommand();

        uint32_t bufferSize = numElements * elementSize;

		// Upload index buffer data.
		uploadCommand->BeginUpload(bufferSize);
		m_indexBuffer = uploadCommand->Update(bufferData, bufferSize);
		uploadCommand->EndUpload();

		// Create the index buffer view.
		m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
        m_indexBufferView.Format = elementSize == sizeof(uint16_t) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
        m_indexBufferView.SizeInBytes = bufferSize;
	}
}