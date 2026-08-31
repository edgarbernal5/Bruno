#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include "DynamicAllocation.h"
#include "GraphicsContext.h"
#include "Bruno/Core/Memory.h"

namespace Bruno
{
    template<typename T>
    class ConstantBuffer
    {
    public:
        ConstantBuffer() = default;
        ~ConstantBuffer() = default;

        // Cero inicializaciones nativas. Todo ocurre al vuelo en O(1)
        void Update(GraphicsContext& context, const T& data) {
            // 1. Calculamos el tamaño alineado (El compilador lo resuelve a una constante)
            /*constexpr uint32_t alignedSize = GetAlignedConstantBufferSize<T>();
            
            // 2. Pedimos el bloque de memoria al allocador lineal de este frame
            DynamicAllocation alloc = context.AllocateDynamicSpace(alignedSize);
            
            // 3. Copiamos los datos C++ directamente a la memoria persistente de la GPU
            std::memcpy(alloc.CPUAddress, &data, sizeof(T));
            
            // 4. Guardamos la dirección para cuando el SceneRenderer quiera "bindearlo"
            m_gpuAddress = alloc.GPUAddress;*/
        }

        [[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress() const
        { 
            return m_gpuAddress; 
        }

    private:
        D3D12_GPU_VIRTUAL_ADDRESS m_gpuAddress = 0;
    };
}
