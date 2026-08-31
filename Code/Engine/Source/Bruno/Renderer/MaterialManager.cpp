#include "brpch.h"
#include "MaterialManager.h"

#include "Material.h"
#include "Bruno/Platform/DirectX/GraphicsDevice.h"
#include "Bruno/Platform/DirectX/UploadContext.h"

namespace Bruno
{
    MaterialManager::MaterialManager(GraphicsDevice& device) : 
        m_device(device)
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

    void MaterialManager::UpdateGPUBuffer(UploadContext& uploadContext)
    {
        if (!m_isDirty || m_materials.empty())
        {
            return;
        }
        
        size_t requiredSize = static_cast<size_t>(m_materials.size() * sizeof(MaterialData));

        // Si el buffer actual es muy pequeño, lo recreamos
        if (requiredSize > m_gpuBufferSize)
        {
            ResizeGPUBuffer(static_cast<size_t>(m_materials.size() * 1.5f)); // Crecer al 150% para evitar recrear seguido
        }

        // Usamos tu UploadContext para subir los datos asíncronamente
        uploadContext.UploadBuffer(m_gpuBuffer.Get(), m_materials.data(), requiredSize, 0);
        
        m_isDirty = false;
    }

    void MaterialManager::ResizeGPUBuffer(uint32_t newElementCount)
    {
        m_gpuBufferSize = newElementCount * sizeof(MaterialData);
        
        // Usar tu abstracción o directamente DX12 para crear un buffer en memoria DEFAULT
        auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        auto desc = CD3DX12_RESOURCE_DESC::Buffer(m_gpuBufferSize);
        
        // Nota: Asegúrate de que tu DescriptorAllocator luego cree un SRV (Shader Resource View)
        // apuntando a este buffer para que el shader lo lea como un StructuredBuffer.
        m_device.GetNativeDevice()->CreateCommittedResource(
            &heapProps, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COMMON, 
            nullptr, IID_PPV_ARGS(&m_gpuBuffer)
        );
    }
}
