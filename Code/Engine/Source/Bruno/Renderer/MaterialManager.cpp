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
        MaterialData defaultMat;
        defaultMat.AlbedoTint = Math::Vector4 {1.0f, 1.0f, 1.0f, 1.0f};
        defaultMat.MetallicFactor = 0.0f;
        defaultMat.RoughnessFactor = 0.5f;
        defaultMat.AlbedoTextureIndex = 0xFFFFFFFF;
        defaultMat.NormalTextureIndex = 0xFFFFFFFF;
        CreateMaterial(defaultMat);
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
        if (!m_isDirty || m_materials.empty())
        {
            return;
        }
        
        size_t requiredSize = m_materials.size() * sizeof(MaterialData);
        if (requiredSize > m_gpuBufferSize)
        {
            ResizeGPUBuffer(static_cast<uint32_t>(m_materials.size() * 1.5f));
        }

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
        m_gpuBufferSize = newElementCount * sizeof(MaterialData);
    
        // 1. Instanciar VRAM PURA (Ahora sí llamamos al constructor correcto)
        m_gpuBuffer = std::make_unique<GpuBuffer>(m_device, m_gpuBufferSize, ResourceState::Common, L"Material_Structured_Buffer");

        // 2. Instanciar RAM (Staging Buffer)
        m_stagingBuffer = std::make_unique<UploadHeap>(m_device, m_gpuBufferSize);
    
        // 3. ACTUALIZAR LA VISTA BINDLESS SRV GLOBAL
    
        // Pedimos un hueco en el Mega Heap (solo la primera vez que creamos el buffer)
        if (m_srvAllocation.Count == 0)
        {
            m_srvAllocation = m_globalSrvHeap.Allocate(1);
        }

        // Configuramos la vista como un StructuredBuffer (Arreglo de MaterialData)
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    
        // ¡Crítico!: DXGI_FORMAT_UNKNOWN es obligatorio para Structured Buffers
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    
        srvDesc.Buffer.NumElements = newElementCount;
        srvDesc.Buffer.StructureByteStride = sizeof(MaterialData);
        srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

        // Sobreescribimos el descriptor en el Mega Heap
        m_device.GetNativeDevice()->CreateShaderResourceView(
            static_cast<ID3D12Resource*>(m_gpuBuffer->GetNativeResource()), 
            &srvDesc, 
            m_srvAllocation.GetCPUHandle() // Tu método mágico de offsets
        );
    }
}
