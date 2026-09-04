#include "brpch.h"
#include "MaterialManager.h"

#include "Material.h"
#include "Bruno/Platform/DirectX/GpuBuffer.h"
#include "Bruno/Platform/DirectX/GraphicsDevice.h"
#include "Bruno/Platform/DirectX/GraphicsContext.h"
#include "Bruno/Platform/DirectX/UploadHeap.h"

namespace Bruno
{
    MaterialManager::MaterialManager(GraphicsDevice& device, DescriptorAllocator& globalSrvHeap) : 
        m_device(device),
        m_globalSrvHeap(globalSrvHeap)
    {
        // Crear un material por defecto (ID 0) para que ninguna entidad se quede sin material
        CreateMaterial(MaterialData());
    }

    uint32_t MaterialManager::CreateMaterial(const MaterialData& data)
    {
        uint32_t index = static_cast<uint32_t>(m_materials.size());
        m_materials.push_back(data);
        m_isDirty = true;
        return index;
    }

    MaterialData& MaterialManager::GetMaterial(uint32_t index)
    {
        return m_materials[index];
    }

    void MaterialManager::MarkAsDirty()
    {
        m_isDirty = true;
    }

    void MaterialManager::UpdateGPUBuffer(GraphicsContext& context)
    {
        if (!m_isDirty || m_materials.empty()) return;
        
        size_t requiredSize = m_materials.size() * sizeof(MaterialData);
        if (requiredSize > m_gpuBufferSize) { /* Resize... */ }

        // Copia directa usando el puntero persistente de tu UploadHeap
        std::memcpy(m_stagingBuffer->GetMappedData(), m_materials.data(), requiredSize);

        // ¡Sinergia total! Ahora usamos tu método TransitionResource con la clase base agnóstica
        context.TransitionResource(m_gpuBuffer.get(), ResourceState::CopyDest);

        // Encolamos la copia asumiendo que agregas un método abstracto CopyBuffer al contexto
        context.CopyBuffer(m_gpuBuffer.get(), m_stagingBuffer.get(), requiredSize);

        context.TransitionResource(m_gpuBuffer.get(), ResourceState::PixelShaderResource);
        
        m_isDirty = false;
    }

    void MaterialManager::ResizeGPUBuffer(uint32_t newElementCount)
    {
        /*m_gpuBufferSize = newElementCount * sizeof(MaterialData);
        auto nativeDevice = m_device.GetNativeDevice();
        
        // 1. Crear el Buffer Principal en VRAM (DEFAULT_HEAP)
        auto defaultHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(m_gpuBufferSize);
        
        nativeDevice->CreateCommittedResource(
            &defaultHeap, D3D12_HEAP_FLAG_NONE, &bufferDesc, 
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, // Nace listo para leerse
            nullptr, IID_PPV_ARGS(&m_gpuBuffer)
        );

        // 2. Crear el Gemelo Puente en RAM (UPLOAD_HEAP)
        auto uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        
        // Importante desmapear el anterior si existía
        if (m_stagingBuffer) m_stagingBuffer->Unmap(0, nullptr);

        nativeDevice->CreateCommittedResource(
            &uploadHeap, D3D12_HEAP_FLAG_NONE, &bufferDesc, 
            D3D12_RESOURCE_STATE_GENERIC_READ, 
            nullptr, IID_PPV_ARGS(&m_stagingBuffer)
        );

        // 3. Mapear permanentemente el puntero para acceso instantáneo
        CD3DX12_RANGE readRange(0, 0); // No leeremos desde la CPU
        m_stagingBuffer->Map(0, &readRange, &m_mappedStagingData);
        
        if (m_srvAllocation.Count == 0)
        {
            m_srvAllocation = m_globalSrvHeap.Allocate(1);
        }
        
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        
        // Obligatorio: DXGI_FORMAT_UNKNOWN para Structured Buffers
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        
        srvDesc.Buffer.NumElements = newElementCount;
        srvDesc.Buffer.StructureByteStride = sizeof(MaterialData);
        srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

        // 3. Inyectamos la vista sobreescribiendo el espacio en el Mega Heap
        m_device.GetNativeDevice()->CreateShaderResourceView(
            m_gpuBuffer.Get(), 
            &srvDesc, 
            m_srvAllocation.GetCPUHandle() // ¡Tu método mágico de offsets!
        );*/
    }
}
