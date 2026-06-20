#include "brpch.h"
#include "UploadContext_Gem.h"

#include "Device.h"

namespace Bruno::DX
{
    UploadContext::UploadContext(DX::GraphicsDevice& device) :
        CommandContext(device, D3D12_COMMAND_LIST_TYPE_COPY)
    {
    }

    void UploadContext::UploadBuffer(ID3D12Resource* destBuffer, const void* data, size_t size)
    {
        // 1. Crear memoria intermedia (Upload Heap)
        auto intermediateBuffer = CreateIntermediateBuffer(size);
        
        // 2. Mapear y copiar de CPU (RAM) a Búfer Intermedio (RAM Visible a GPU)
        void* mappedData = nullptr;
        ThrowIfFailed(intermediateBuffer->Map(0, nullptr, &mappedData));
        memcpy(mappedData, data, size);
        intermediateBuffer->Unmap(0, nullptr);

        // 3. Grabar el comando para que la GPU mueva los datos al Destino Final (VRAM)
        m_commandList->CopyBufferRegion(destBuffer, 0, intermediateBuffer.Get(), 0, size);

        // 4. Retener el búfer para que no se destruya al salir de esta función
        m_intermediateBuffers.push_back(intermediateBuffer);
    }

    void UploadContext::UploadTexture(ID3D12Resource* destTexture, const D3D12_SUBRESOURCE_DATA& subResourceData)
    {
        UINT64 requiredSize = 0;
        auto textureDesc = destTexture->GetDesc();
        m_device.GetNativeDevice()->GetCopyableFootprints(
            &textureDesc, 0, 1, 0, nullptr, nullptr, nullptr, &requiredSize);

        auto intermediateBuffer = CreateIntermediateBuffer(requiredSize);

        // Actualizamos subrecursos usando la metadata que DirectXTK ya calculó
        UpdateSubresources(
            m_commandList.Get(), 
            destTexture, 
            intermediateBuffer.Get(), 
            0, 0, 1, 
            &subResourceData
        );

        m_intermediateBuffers.push_back(intermediateBuffer);
    }

    void UploadContext::ClearGarbage()
    {
        // Vaciamos el vector. Los ComPtr llamarán automáticamente a ->Release()
        // liberando la memoria de la GPU.
        m_intermediateBuffers.clear(); 
    }

    Microsoft::WRL::ComPtr<ID3D12Resource> UploadContext::CreateIntermediateBuffer(UINT64 sizeInBytes)
    {
        Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
        
        auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeInBytes);

        ThrowIfFailed(m_device.GetNativeDevice()->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ, // Los Upload Heaps DEBEN estar en este estado
            nullptr,
            IID_PPV_ARGS(&buffer)
        ));

        return buffer;
    }
}
