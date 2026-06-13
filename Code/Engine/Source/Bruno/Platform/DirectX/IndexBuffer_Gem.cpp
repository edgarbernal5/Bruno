#include "brpch.h"
#include "IndexBuffer_Gem.h"

namespace Bruno::DX
{
    IndexBuffer::IndexBuffer(GraphicsDevice& device, ID3D12GraphicsCommandList* cmdList, const std::vector<uint32_t>& indices)
        : m_indicesCount(static_cast<uint32_t>(indices.size())) {
        
        auto nativeDevice = device.GetNativeDevice();
        size_t bufferSize = indices.size() * sizeof(uint32_t);

        // 1. Crear el Default Heap (VRAM pura)
        D3D12_HEAP_PROPERTIES defaultHeapProps{D3D12_HEAP_TYPE_DEFAULT};
        D3D12_RESOURCE_DESC bufferDesc{D3D12_RESOURCE_DIMENSION_BUFFER, 0, bufferSize, 1, 1, 1, DXGI_FORMAT_UNKNOWN, 1, 0, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_NONE};
        nativeDevice->CreateCommittedResource(&defaultHeapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_defaultBuffer));

        // 2. Crear el Upload Heap (Puente CPU-GPU)
        D3D12_HEAP_PROPERTIES uploadHeapProps{D3D12_HEAP_TYPE_UPLOAD};
        nativeDevice->CreateCommittedResource(&uploadHeapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_uploadBuffer));

        // 3. Copiar datos a la memoria de subida (Upload)
        void* pData = nullptr;
        m_uploadBuffer->Map(0, nullptr, &pData);
        memcpy(pData, indices.data(), bufferSize);
        m_uploadBuffer->Unmap(0, nullptr);

        // 4. Ordenar a la GPU copiar del Upload al Default
        cmdList->CopyBufferRegion(m_defaultBuffer.Get(), 0, m_uploadBuffer.Get(), 0, bufferSize);

        // Transición de estado: de COPY_DEST a INDEX_BUFFER
        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = m_defaultBuffer.Get();
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_INDEX_BUFFER;
        cmdList->ResourceBarrier(1, &barrier);

        // 5. Preparar la vista
        m_view.BufferLocation = m_defaultBuffer->GetGPUVirtualAddress();
        m_view.Format = DXGI_FORMAT_R32_UINT;
        m_view.SizeInBytes = static_cast<UINT>(bufferSize);
    }
}