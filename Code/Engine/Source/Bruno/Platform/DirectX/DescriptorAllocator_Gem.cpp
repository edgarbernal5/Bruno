#include "brpch.h"
#include "DescriptorAllocator_Gem.h"

namespace Bruno::DX 
{
    DescriptorAllocator::DescriptorAllocator(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t capacity)
        : m_heapType(type)
        , m_capacity(capacity)
        , m_allocatedCount(0)
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.NumDescriptors = m_capacity;
        heapDesc.Type = m_heapType;
        
        // Regla de Oro DX12: Si son texturas/buffers para el Shader (CBV_SRV_UAV), 
        // el heap TIENE que ser visible para la GPU.
        if (type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV || type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER) {
            heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        } else {
            heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        }
        heapDesc.NodeMask = 0;

        HRESULT hr = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_heap));
        if (FAILED(hr)) {
            throw std::runtime_error("Fallo crítico: No se pudo crear el Descriptor Heap.");
        }

        // 1. Guardamos el tamaño del salto en memoria (Varía según el fabricante AMD/NVIDIA)
        m_descriptorSize = device->GetDescriptorHandleIncrementSize(m_heapType);
        
        // 2. Guardamos las direcciones de inicio
        m_cpuStart = m_heap->GetCPUDescriptorHandleForHeapStart();
        
        if (heapDesc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) {
            m_gpuStart = m_heap->GetGPUDescriptorHandleForHeapStart();
        } else {
            m_gpuStart.ptr = 0; // Heaps como los Render Targets (RTV) no tienen acceso a la GPU
        }
    }

    DescriptorAllocation DescriptorAllocator::Allocate(uint32_t count) 
    {
        if (m_allocatedCount + count > m_capacity) {
            // En un motor AAA real, aquí instanciaríamos un nuevo Heap y lo encadenaríamos,
            // pero bajo el principio KISS, lanzar una excepción para redimensionar la capacidad es perfecto hoy.
            throw std::runtime_error("DescriptorAllocator se ha quedado sin capacidad.");
        }

        DescriptorAllocation allocation;
        allocation.Index = m_allocatedCount;
        
        // Magia aritmética: Avanzamos los punteros sumando bytes exactos.
        // Costo de CPU: Prácticamente 0 ciclos de reloj.
        allocation.CPU.ptr = m_cpuStart.ptr + (static_cast<SIZE_T>(m_allocatedCount) * m_descriptorSize);
        
        if (m_gpuStart.ptr != 0) {
            allocation.GPU.ptr = m_gpuStart.ptr + (static_cast<SIZE_T>(m_allocatedCount) * m_descriptorSize);
        } else {
            allocation.GPU.ptr = 0;
        }

        // Avanzamos el cabezal de memoria
        m_allocatedCount += count;
        
        return allocation;
    }
}