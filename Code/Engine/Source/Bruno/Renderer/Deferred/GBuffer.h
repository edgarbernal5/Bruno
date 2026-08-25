#pragma once
#include <memory>

namespace Bruno
{
    class GraphicsDevice;
    class Texture2D;
    class DepthBuffer;
    class DescriptorAllocator;

    class GBuffer {
    public:
        GBuffer() = default;
        ~GBuffer() = default;

        // Prohibimos copias para evitar duplicar recursos de GPU accidentalmente
        GBuffer(const GBuffer&) = delete;
        GBuffer& operator=(const GBuffer&) = delete;

        // Inicializa las texturas al arrancar el motor
        void Initialize(GraphicsDevice& device, DescriptorAllocator& srvHeap, DescriptorAllocator& rtvHeap, uint32_t width, uint32_t height);
        
        // Se llama cuando la ventana (y el SwapChain) cambian de tamaño
        void Resize(GraphicsDevice& device, DescriptorAllocator& srvHeap, DescriptorAllocator& rtvHeap, uint32_t width, uint32_t height);

        // --- Getters para el Render Loop ---
        Texture2D* GetAlbedoMetalness() const { return m_albedoMetalness.get(); }
        Texture2D* GetNormalRoughness() const { return m_normalRoughness.get(); }
        Texture2D* GetPosition() const        { return m_position.get(); }
        DepthBuffer* GetDepth() const         { return m_depthBuffer.get(); }

    private:
        std::unique_ptr<Texture2D> m_albedoMetalness; // SV_Target0
        std::unique_ptr<Texture2D> m_normalRoughness; // SV_Target1
        std::unique_ptr<Texture2D> m_position;        // SV_Target2
        
        std::unique_ptr<DepthBuffer> m_depthBuffer;   // Z-Buffer
    };

}