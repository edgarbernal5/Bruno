#include "brpch.h"
#include "IndexBuffer_Gem.h"

#include "UploadContext_Gem.h"

namespace Bruno::DX
{
    IndexBuffer::IndexBuffer(GraphicsDevice& device, UploadContext& uploadContext, const std::vector<uint32_t>& indices)
    : m_indicesCount(static_cast<uint32_t>(indices.size())) 
    {
        auto nativeDevice = device.GetNativeDevice();
        size_t bufferSize = indices.size() * sizeof(uint32_t);

        // 1. Crear SOLAMENTE el Default Heap (VRAM pura)
        auto defaultHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

        ThrowIfFailed(nativeDevice->CreateCommittedResource(
            &defaultHeapProps, 
            D3D12_HEAP_FLAG_NONE, 
            &bufferDesc, 
            D3D12_RESOURCE_STATE_COMMON, // ¡Nace en COMMON para evitar los warnings!
            nullptr, 
            IID_PPV_ARGS(&m_defaultBuffer)));

        // 2. Delegar TODA la lógica de subida y memoria temporal a nuestro UploadContext
        // (Esto graba el CopyBufferRegion y retiene la basura temporal por nosotros)
        uploadContext.UploadBuffer(m_defaultBuffer.Get(), indices.data(), bufferSize);

        // 3. Preparar la vista (El "Ticket" para el RenderLoop)
        m_view.BufferLocation = m_defaultBuffer->GetGPUVirtualAddress();
        m_view.Format = DXGI_FORMAT_R32_UINT;
        m_view.SizeInBytes = static_cast<UINT>(bufferSize);
    
        // Adiós barreras manuales, la promoción de DX12 se encarga del resto.
    }
}