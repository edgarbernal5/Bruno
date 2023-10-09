#include "brpch.h"
#include "VertexBuffer.h"

#include "GraphicsDevice.h"
#include "UploadCommand.h"

namespace Bruno
{
	VertexBuffer::VertexBuffer(uint32_t numVertices, const void* bufferData, uint32_t vertexStride) :
		m_numVertices(numVertices),
		m_vertexStride(vertexStride),
		m_vertexBufferView{}
	{
		//GraphicsDevice* device = Graphics::GetDevice();
		//auto uploadCommand = device->GetUploadCommand();

		//uint32_t bufferSize = numVertices * vertexStride;

		//// Upload vertex buffer data.
		//uploadCommand->BeginUpload(bufferSize);
		//m_vertexBuffer = uploadCommand->Update(bufferData, bufferSize);
		//uploadCommand->EndUpload();

		// Create the vertex buffer view.
		//m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
  //      m_vertexBufferView.SizeInBytes = vertexStride * numVertices;
  //      m_vertexBufferView.StrideInBytes = vertexStride;
	}
}