#include "brpch.h"
#include "VertexBuffer_Gem.h"

#include "UploadContext_Gem.h"
#include "Device.h"

namespace Bruno::DX {

    VertexBuffer::VertexBuffer(GraphicsDevice& device, DX::UploadContext& uploadContext, const void* data, size_t vertexCount, size_t vertexStride) 
    {
        size_t byteSize = vertexCount * vertexStride;
        auto nativeDevice = device.GetNativeDevice();

        // 1. Crear SOLAMENTE el Default Heap (VRAM pura)
        auto defaultHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(byteSize);

        ThrowIfFailed(nativeDevice->CreateCommittedResource(
            &defaultHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_COMMON, // ¡Sin warnings #1328!
            nullptr,
            IID_PPV_ARGS(&m_defaultBuffer)));

        // 2. Usar el motor DMA (UploadContext) para transferir los datos
        uploadContext.UploadBuffer(m_defaultBuffer.Get(), data, byteSize);

        // 3. Preparar la Vista (El "Ticket" para el RenderLoop)
        m_view.BufferLocation = m_defaultBuffer->GetGPUVirtualAddress();
        m_view.StrideInBytes = static_cast<UINT>(vertexStride);
        m_view.SizeInBytes = static_cast<UINT>(byteSize);
    }

    void VertexBuffer::FreeUploadBuffer()
    {
        // Llama a esto SOLO cuando tu motor esté seguro de que la CommandList se ejecutó.
        m_uploadBuffer.Reset();
    }
}