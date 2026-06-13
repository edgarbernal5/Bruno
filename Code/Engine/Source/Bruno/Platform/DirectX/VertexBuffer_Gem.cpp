#include "brpch.h"
#include "VertexBuffer_Gem.h"

namespace Bruno::DX {

    VertexBuffer::VertexBuffer(ID3D12Device* device, 
                               ID3D12GraphicsCommandList* cmdList, 
                               const void* data, 
                               size_t vertexCount, 
                               size_t vertexStride) 
    {
        size_t byteSize = vertexCount * vertexStride;

        // 1. Crear el buffer de Destino en VRAM pura (Default Heap)
        D3D12_HEAP_PROPERTIES defaultHeapProps = {};
        defaultHeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

        D3D12_RESOURCE_DESC bufferDesc = {};
        bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        bufferDesc.Width = byteSize;
        bufferDesc.Height = 1;
        bufferDesc.DepthOrArraySize = 1;
        bufferDesc.MipLevels = 1;
        bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
        bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        bufferDesc.SampleDesc.Count = 1;

        HRESULT hr = device->CreateCommittedResource(
            &defaultHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_COPY_DEST, // ¡Importante! Nace listo para recibir datos
            nullptr,
            IID_PPV_ARGS(&m_defaultBuffer));

        if (FAILED(hr)) throw std::runtime_error("Fallo al crear Vertex Default Buffer.");

        // 2. Crear el buffer temporal en RAM compartida (Upload Heap)
        D3D12_HEAP_PROPERTIES uploadHeapProps = {};
        uploadHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

        hr = device->CreateCommittedResource(
            &uploadHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ, // Nace listo para ser leído por la GPU
            nullptr,
            IID_PPV_ARGS(&m_uploadBuffer));

        if (FAILED(hr)) throw std::runtime_error("Fallo al crear Vertex Upload Buffer.");

        // 3. Copiar la data de tu std::vector de C++ al Upload Buffer
        void* mappedData = nullptr;
        m_uploadBuffer->Map(0, nullptr, &mappedData);
        memcpy(mappedData, data, byteSize);
        m_uploadBuffer->Unmap(0, nullptr);

        // 4. Grabar en la GPU la orden de transferir del Upload al Default
        cmdList->CopyBufferRegion(m_defaultBuffer.Get(), 0, m_uploadBuffer.Get(), 0, byteSize);

        // 5. Insertar una Barrera (Barrier) para avisar a la GPU que la copia terminó
        // y ahora el buffer se usará para alimentar vértices al shader
        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = m_defaultBuffer.Get();
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

        cmdList->ResourceBarrier(1, &barrier);

        // 6. Preparar la Vista (El "Ticket" que le pasaremos al render loop)
        m_view.BufferLocation = m_defaultBuffer->GetGPUVirtualAddress();
        m_view.StrideInBytes = static_cast<UINT>(vertexStride);
        m_view.SizeInBytes = static_cast<UINT>(byteSize);
    }

    void VertexBuffer::FreeUploadBuffer() {
        // Llama a esto SOLO cuando tu motor esté seguro de que la CommandList se ejecutó.
        m_uploadBuffer.Reset();
    }
}