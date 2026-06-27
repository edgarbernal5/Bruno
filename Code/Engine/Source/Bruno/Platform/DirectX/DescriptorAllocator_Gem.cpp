#include "brpch.h"
#include "DescriptorAllocator_Gem.h"

#include "Device.h"

namespace Bruno::DX 
{
    DescriptorAllocator::DescriptorAllocator(DX::GraphicsDevice& device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t capacity, bool isShaderVisible)
    : m_heapType(type), m_capacity(capacity), m_allocatedCount(0)
    {
        auto nativeDevice = device.GetNativeDevice();
        
        // 1. Describir cómo queremos nuestro Heap
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.NumDescriptors = capacity;
        heapDesc.Type = type;
    
        // REGLA DE ORO: Si es para texturas/CBVs que se usan en el Draw(), DEBE ser Shader Visible
        heapDesc.Flags = isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        heapDesc.NodeMask = 0; // 0 significa que asume un solo adaptador (GPU)

        // 2. Crear el Heap nativo en la GPU
        HRESULT hr = nativeDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_heap));
        if (FAILED(hr))
        {
            throw std::runtime_error("Fallo al crear el Descriptor Heap.");
        }

        // 3. Obtener el tamaño en bytes de cada descriptor para este tipo de Heap (varía según GPU)
        m_descriptorSize = nativeDevice->GetDescriptorHandleIncrementSize(type);

        // 4. Guardar los cabezales de inicio (para la magia matemática de Allocate)
        m_cpuStart = m_heap->GetCPUDescriptorHandleForHeapStart();
    
        if (isShaderVisible) {
            m_gpuStart = m_heap->GetGPUDescriptorHandleForHeapStart();
        } else {
            m_gpuStart.ptr = 0; // Si no es shader visible, el GPU handle no es válido
        }
    }

    DescriptorAllocation DescriptorAllocator::Allocate(uint32_t count) 
    {
        if (m_allocatedCount + count > m_capacity)
        {
            // En un motor AAA real, aquí instanciaríamos un nuevo Heap y lo encadenaríamos,
            // pero bajo el principio KISS, lanzar una excepción para redimensionar la capacidad es perfecto hoy.
            throw std::runtime_error("DescriptorAllocator se ha quedado sin capacidad.");
        }

        DescriptorAllocation allocation;
        allocation.Index = m_allocatedCount;
        allocation.Count = count;
        allocation.DescriptorSize = m_descriptorSize;
        
        // Magia aritmética: Avanzamos los punteros sumando bytes exactos.
        // Costo de CPU: Prácticamente 0 ciclos de reloj.
        allocation.CPU.ptr = m_cpuStart.ptr + (static_cast<SIZE_T>(m_allocatedCount) * m_descriptorSize);
        
        if (m_gpuStart.ptr != 0)
        {
            allocation.GPU.ptr = m_gpuStart.ptr + (static_cast<SIZE_T>(m_allocatedCount) * m_descriptorSize);
        } 
        else
        {
            allocation.GPU.ptr = 0;
        }

        // Avanzamos el cabezal de memoria
        m_allocatedCount += count;
        
        return allocation;
    }
}