#pragma once

#include "D3DHelpers.h"
#include <wrl/client.h>
#include <vector>

namespace Bruno::DX 
{
    class UploadHeap 
    {
    public:
        // Crea un buffer de tamaño específico en la memoria de subida
        UploadHeap(ID3D12Device* device, UINT64 size);
        ~UploadHeap() = default;

        // Copia datos de CPU a este buffer
        void UploadData(const void* data, UINT64 size, UINT64 offset = 0);

        [[nodiscard]] ID3D12Resource* GetResource() const { return m_uploadBuffer.Get(); }
        [[nodiscard]] UINT64 GetSize() const { return m_size; }

    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> m_uploadBuffer;
        void* m_mappedData = nullptr; // Puntero directo a la memoria de la CPU
        UINT64 m_size = 0;
    };
}