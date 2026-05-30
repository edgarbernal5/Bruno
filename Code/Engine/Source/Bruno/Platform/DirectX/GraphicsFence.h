#pragma once

#include "D3DHelpers.h"

namespace Bruno::DX
{
    class GraphicsDevice;

    class GraphicsFence 
    {
    public:
        explicit GraphicsFence(GraphicsDevice& device);
        ~GraphicsFence();

        // C++17: Evitar copias. Un Fence es un recurso único de hardware.
        GraphicsFence(const GraphicsFence&) = delete;
        GraphicsFence& operator=(const GraphicsFence&) = delete;

        // Inserta una señal en la cola de la GPU y devuelve el valor esperado.
        uint64_t Signal(ID3D12CommandQueue* commandQueue);

        // Bloquea la CPU hasta que la GPU alcance el valor especificado.
        void WaitForValue(uint64_t expectedFenceValue);

        // Utilidad extrema: Detiene toda la CPU hasta que la GPU se vacíe por completo.
        void Flush(ID3D12CommandQueue* commandQueue);

        [[nodiscard]] uint64_t GetCompletedValue() const { return m_fence->GetCompletedValue(); }

    private:
        Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
        uint64_t m_currentValue{ 0 };
        HANDLE m_fenceEvent{ nullptr }; 
    };
}