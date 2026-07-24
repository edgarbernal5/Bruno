#pragma once

#include "D3DHelpers.h"

namespace Bruno
{
    class GraphicsDevice;
    
    class RenderContext 
    {
    public:
        explicit RenderContext(GraphicsDevice& device);
        ~RenderContext() = default;

        // Inicia la grabación del frame actual
        void Begin();
        
        // Cierra la lista y la envía a la cola de la GPU
        void Execute();

        // Expone la lista para que otros subsistemas (como Berta UI) dibujen
        [[nodiscard]] ID3D12GraphicsCommandList* GetCommandList() const { return m_commandList.Get(); }

    private:
        GraphicsDevice& m_device;
        
        // En un motor AAA, tendrías un Allocator por cada Frame en vuelo,
        // pero para empezar, KISS: 1 Allocator y 1 Command List.
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
    };
}