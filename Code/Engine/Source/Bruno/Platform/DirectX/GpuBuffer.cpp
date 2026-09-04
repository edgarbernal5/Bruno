#include "brpch.h"
#include "GpuBuffer.h"

namespace Bruno {

    // Constructor Dinámico
    GpuBuffer::GpuBuffer(GraphicsDevice& device, size_t sizeInBytes, const std::wstring& name) 
        : m_size(sizeInBytes), m_isDynamic(true)
    {
        m_name = name;
        m_currentState = ResourceState::GenericRead; // ¡Diferente al COMMON del estático!

        auto nativeDevice = device.GetNativeDevice();
        auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD); // Memoria visible para CPU y GPU
        auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(m_size);

        ThrowIfFailed(nativeDevice->CreateCommittedResource(
            &heapProps, 
            D3D12_HEAP_FLAG_NONE, 
            &bufferDesc, 
            D3D12_RESOURCE_STATE_GENERIC_READ, 
            nullptr, 
            IID_PPV_ARGS(&m_resource)
        ));

        m_resource->SetName(m_name.c_str());

        // Mapeo Persistente: A diferencia de mapear/desmapear cada vez, retenemos el puntero para costo de CPU 0.
        CD3DX12_RANGE readRange(0, 0); 
        ThrowIfFailed(m_resource->Map(0, &readRange, &m_mappedData));
    }

    // Constructor Estático
    GpuBuffer::GpuBuffer(GraphicsDevice& device, UploadContext& uploadContext, const void* data, size_t sizeInBytes, const std::wstring& name) 
        : m_size(sizeInBytes), m_isDynamic(false)
    {
        m_name = name;
        m_currentState = ResourceState::Common; // Nace en COMMON para evitar los warnings

        auto nativeDevice = device.GetNativeDevice();
        
        // Crear SOLAMENTE el Default Heap (VRAM pura)
        auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(m_size);

        ThrowIfFailed(nativeDevice->CreateCommittedResource(
            &heapProps, 
            D3D12_HEAP_FLAG_NONE, 
            &bufferDesc, 
            D3D12_RESOURCE_STATE_COMMON, 
            nullptr, 
            IID_PPV_ARGS(&m_resource)
        ));

        m_resource->SetName(m_name.c_str());

        // Delegar TODA la lógica de subida y memoria temporal a nuestro UploadContext
        uploadContext.UploadBuffer(m_resource.Get(), data, m_size);
    }

    GpuBuffer::~GpuBuffer()
    {
        if (m_isDynamic && m_mappedData) 
        {
            m_resource->Unmap(0, nullptr);
        }
    }

    void GpuBuffer::Update(const void* data, size_t updateSize)
    {
        if (!m_isDynamic)
        {
            throw std::runtime_error("¡No puedes actualizar un buffer estatico desde la CPU de esta manera!");
        }
        
        if (updateSize > m_size)
        {
            throw std::out_of_range("El tamaño de actualizacion excede la capacidad del buffer.");
        }

        // Map ya no necesita sincronizacion ni desmapeo si solo escribimos en el puntero persistente
        ::std::memcpy(m_mappedData, data, updateSize);
    }
}