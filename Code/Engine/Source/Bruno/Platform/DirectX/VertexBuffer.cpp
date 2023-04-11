#include "brpch.h"
#include "VertexBuffer.h"

#include "GraphicsDevice.h"

namespace Bruno
{
	VertexBuffer::VertexBuffer(uint32_t numVertices, const void* bufferData, uint32_t vertexStride) :
		m_numVertices(numVertices),
		m_vertexStride(vertexStride),
		m_vertexBufferView{}
	{
		GraphicsDevice* device = Graphics::GetDevice();

		// Upload vertex buffer data.
		Microsoft::WRL::ComPtr<ID3D12Resource> intermediateVertexBuffer;

        D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;
        uint32_t bufferSize = numVertices * vertexStride;

        // Create a committed resource for the GPU resource in a default heap.
        ThrowIfFailed(device->GetD3DDevice()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(bufferSize, flags),
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(m_vertexBuffer.ReleaseAndGetAddressOf())));

        // Create an committed resource for the upload.
        if (bufferData)
        {
            ThrowIfFailed(device->GetD3DDevice()->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(intermediateVertexBuffer.ReleaseAndGetAddressOf())));

            D3D12_SUBRESOURCE_DATA subresourceData = {};
            subresourceData.pData = bufferData;
            subresourceData.RowPitch = bufferSize;
            subresourceData.SlicePitch = subresourceData.RowPitch;

            UpdateSubresources(device->GetCommandQueue()->GetCommandList(),
                m_vertexBuffer.Get(), intermediateVertexBuffer.Get(),
                0, 0, 1, &subresourceData);

        }
		// Create the vertex buffer view.
		m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
        m_vertexBufferView.SizeInBytes = vertexStride * numVertices;
        m_vertexBufferView.StrideInBytes = vertexStride;
	}
}