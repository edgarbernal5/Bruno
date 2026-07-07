#include "brpch.h"
#include "VertexBuffer_Gem.h"

#include "UploadContext_Gem.h"
#include "Device.h"

namespace Bruno::DX
{
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
            IID_PPV_ARGS(&m_buffer)));

        // 2. Usar el motor DMA (UploadContext) para transferir los datos
        uploadContext.UploadBuffer(m_buffer.Get(), data, byteSize);

        // 3. Preparar la Vista (El "Ticket" para el RenderLoop)
        m_view.BufferLocation = m_buffer->GetGPUVirtualAddress();
        m_view.StrideInBytes = static_cast<UINT>(vertexStride);
        m_view.SizeInBytes = static_cast<UINT>(byteSize);
    }

    VertexBuffer::VertexBuffer(GraphicsDevice& device, size_t bufferSize, uint32_t stride, bool isDynamic) :
        m_isDynamic(isDynamic)
    {
        auto nativeDevice = device.GetNativeDevice();

        // 1. OBLIGATORIO: Heap de tipo UPLOAD para poder hacer Map()
        auto uploadHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

        ThrowIfFailed(nativeDevice->CreateCommittedResource(
            &uploadHeapProps, 
            D3D12_HEAP_FLAG_NONE, 
            &bufferDesc, 
            D3D12_RESOURCE_STATE_GENERIC_READ, // 2. OBLIGATORIO: Estado Generic Read
            nullptr, 
            IID_PPV_ARGS(&m_buffer)));

        m_view.BufferLocation = m_buffer->GetGPUVirtualAddress();
        m_view.SizeInBytes = static_cast<UINT>(bufferSize);
        m_view.StrideInBytes = static_cast<UINT>(stride);
    }

    void VertexBuffer::Update(const void* data, size_t size)
    {
        if (!m_isDynamic)
        {
            return; // O lanzar un error: "¡No puedes actualizar un buffer estático!"
        }
        
        void* mappedData = nullptr;
    
        CD3DX12_RANGE readRange(0, 0); // No vamos a leer, solo escribir
        ThrowIfFailed(m_buffer->Map(0, &readRange, &mappedData));
    
        ::memcpy(mappedData, data, size);
    
        m_buffer->Unmap(0, nullptr);
    
        // Actualizamos el SizeInBytes por si en este frame dibujamos menos índices que la capacidad total
        m_view.SizeInBytes = static_cast<UINT>(size);
    }

    UINT VertexBuffer::GetSizeInBytes() const
    {
        return m_view.SizeInBytes;
    }
}
