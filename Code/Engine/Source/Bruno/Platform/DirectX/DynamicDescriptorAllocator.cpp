#include "brpch.h"
#include "DynamicDescriptorAllocator.h"

#include "GraphicsDevice.h"

namespace Bruno
{
    DynamicDescriptorAllocator::DynamicDescriptorAllocator(GraphicsDevice& device, uint32_t capacity) : 
        DescriptorAllocator(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, capacity, true)
    {
    }

    void DynamicDescriptorAllocator::Reset()
    {
        // Costo de CPU: 0. Solo reiniciamos el offset.
        m_allocatedCount = 0;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE DynamicDescriptorAllocator::CopyDescriptor(GraphicsDevice& device,
        D3D12_CPU_DESCRIPTOR_HANDLE cpuStagingDescriptor)
    {
        // 1. Reservamos 1 hueco en nuestro heap dinámico actual
        DescriptorAllocation alloc = Allocate(1);

        // 2. Le pedimos a la API nativa que copie el contenido del Staging al Dinámico
        device.GetNativeDevice()->CopyDescriptorsSimple(
            1,                      // Cuántos descriptores copiar
            alloc.CPU,              // Destino (en el heap dinámico)
            cpuStagingDescriptor,   // Origen (del Staging heap)
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
        );

        // 3. Devolvemos la dirección en GPU para que el CommandList la use
        return alloc.GPU;
    }
}
