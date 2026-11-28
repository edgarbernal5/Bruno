#pragma once

#include <d3d12.h>

#include "ConstantBufferBase.h"
#include "DynamicAllocation.h"
#include "GraphicsContext.h"
#include "Bruno/Core/Memory.h"

namespace Bruno
{
    template<typename T>
    class ConstantBuffer : public ConstantBufferBase
    {
    public:
        ConstantBuffer() = default;
        ~ConstantBuffer() = default;

        // Cero inicializaciones nativas. Todo ocurre al vuelo en O(1)
        void Update(GraphicsContext& context, const T& data)
        {
            constexpr uint32_t alignedSize = GetAlignedConstantBufferSize<T>();
            
            DynamicAllocation alloc = context.AllocateDynamicSpace(alignedSize);
            
            std::memcpy(alloc.CPUAddress, &data, sizeof(T));
            
            // Guardamos la dirección en la variable protegida de la clase base
            m_gpuAddress = alloc.GPUAddress;
        }
    };
}
