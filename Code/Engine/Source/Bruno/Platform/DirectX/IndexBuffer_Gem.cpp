#include "brpch.h"
#include "IndexBuffer_Gem.h"

#include "UploadContext_Gem.h"

namespace Bruno::DX
{
    IndexBuffer::IndexBuffer(GraphicsDevice& device, UploadContext& uploadContext, const std::vector<uint32_t>& indices) :
        m_indicesCount(static_cast<uint32_t>(indices.size()))
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
            IID_PPV_ARGS(&m_buffer)));

        // 2. Delegar TODA la lógica de subida y memoria temporal a nuestro UploadContext
        // (Esto graba el CopyBufferRegion y retiene la basura temporal por nosotros)
        uploadContext.UploadBuffer(m_buffer.Get(), indices.data(), bufferSize);

        // 3. Preparar la vista (El "Ticket" para el RenderLoop)
        m_view.BufferLocation = m_buffer->GetGPUVirtualAddress();
        m_view.Format = DXGI_FORMAT_R32_UINT;
        m_view.SizeInBytes = static_cast<UINT>(bufferSize);
    
        // Adiós barreras manuales, la promoción de DX12 se encarga del resto.
    }

    // Implementación del nuevo Constructor Dinámico
    IndexBuffer::IndexBuffer(GraphicsDevice& device, size_t bufferSize, bool isDynamic) :
        m_isDynamic(isDynamic), m_indicesCount(0)
    {
        auto nativeDevice = device.GetNativeDevice();

        // 1. Crear en el Upload Heap (Memoria visible para CPU y GPU)
        auto uploadHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

        ThrowIfFailed(nativeDevice->CreateCommittedResource(
            &uploadHeapProps, 
            D3D12_HEAP_FLAG_NONE, 
            &bufferDesc, 
            D3D12_RESOURCE_STATE_GENERIC_READ, // ¡Diferente al COMMON del estático!
            nullptr, 
            IID_PPV_ARGS(&m_buffer)));

        // 2. Preparar la vista
        m_view.BufferLocation = m_buffer->GetGPUVirtualAddress();
        m_view.Format = DXGI_FORMAT_R32_UINT;
        m_view.SizeInBytes = static_cast<UINT>(bufferSize);
    }

    // Implementación del método Update
    void IndexBuffer::Update(const void* data, size_t size)
    {
        if (!m_isDynamic) return; // O lanzar un error: "¡No puedes actualizar un buffer estático!"

        void* mappedData = nullptr;
    
        // Map no necesita sincronización compleja si solo escribimos en él
        ThrowIfFailed(m_buffer->Map(0, nullptr, &mappedData));
    
        ::memcpy(mappedData, data, size);
    
        m_buffer->Unmap(0, nullptr);
    
        // Actualizamos el SizeInBytes por si en este frame dibujamos menos índices que la capacidad total
        m_view.SizeInBytes = static_cast<UINT>(size);
    }

    UINT IndexBuffer::GetSizeInBytes() const
    {
        return m_view.SizeInBytes;
    }
}
