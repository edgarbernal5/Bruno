#pragma once

#include "GraphicsResource.h"
#include "GraphicsDevice.h"
#include "UploadContext.h"
#include <wrl/client.h>
#include <d3d12.h>
#include <stdexcept>
#include <string>

namespace Bruno {

    class GpuBuffer : public GraphicsResource
    {
    public:
        // 1. Constructor Dinámico (UPLOAD_HEAP): Para geometría que cambia por frame (Ej: PrimitiveBatch)
        GpuBuffer(GraphicsDevice& device, size_t sizeInBytes, const std::wstring& name = L"Dynamic_Buffer");

        // 2. Constructor Estático (DEFAULT_HEAP): Para geometría inmutable (Ej: Modelos 3D)
        GpuBuffer(GraphicsDevice& device, UploadContext& uploadContext, const void* data, size_t sizeInBytes, const std::wstring& name = L"Static_Buffer");

        virtual ~GpuBuffer();

        // Prohibimos copias para proteger el recurso nativo
        GpuBuffer(const GpuBuffer&) = delete;
        GpuBuffer& operator=(const GpuBuffer&) = delete;

        // Método unificado para actualizar datos. Protegido internamente contra escrituras inválidas.
        void Update(const void* data, size_t sizeInBytes);
        
        [[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress() const { return m_resource->GetGPUVirtualAddress(); }
        [[nodiscard]] size_t GetSizeInBytes() const { return m_size; }
        [[nodiscard]] bool IsDynamic() const { return m_isDynamic; }

    protected:
        size_t m_size = 0;
        bool m_isDynamic = false;
        
        // Puntero mapeado permanentemente. Elimina el costo del Map/Unmap por frame de tu antigua implementación
        void* m_mappedData = nullptr; 
    };
}