#include "brpch.h"
#include "GBuffer.h"

#include "Bruno/Platform/DirectX/DepthBuffer.h"
#include "Bruno/Platform/DirectX/Texture2D.h"
#include "Bruno/Renderer/RHITypes.h"

namespace Bruno
{
    void GBuffer::Initialize(GraphicsDevice& device, DescriptorAllocator& srvHeap, DescriptorAllocator& rtvHeap, uint32_t width, uint32_t height)
    {
        // 1. Albedo + Metalness (R8G8B8A8)
        // RGB = Color Base, A = Factor Metálico
        m_albedoMetalness = std::make_unique<Texture2D>(
            device, 
            width, height, 
            TextureFormat::R8G8B8A8_Unorm, // Usando tu enum agnóstico
            srvHeap, rtvHeap, 
            L"GBuffer_AlbedoMetal"
        );

        // 2. Normal + Roughness (R16G16B16A16_FLOAT)
        // Alta precisión necesaria para evitar artefactos (banding) en la iluminación y normales correctas
        m_normalRoughness = std::make_unique<Texture2D>(
            device, 
            width, height, 
            TextureFormat::R16G16B16A16_Float, 
            srvHeap, rtvHeap, 
            L"GBuffer_NormalRough"
        );

        // 3. Posición en el Mundo (R16G16B16A16_FLOAT)
        // Float de 16-bits suele ser suficiente, pero si tu mundo es masivo, podrías necesitar 32-bits
        m_position = std::make_unique<Texture2D>(
            device, 
            width, height, 
            TextureFormat::R16G16B16A16_Float, 
            srvHeap, rtvHeap, 
            L"GBuffer_Position"
        );

        // 4. Depth / Stencil Buffer
        m_depthBuffer = std::make_unique<DepthBuffer>(device, width, height);
    }

    void GBuffer::Resize(GraphicsDevice& device, DescriptorAllocator& srvHeap, DescriptorAllocator& rtvHeap, uint32_t width, uint32_t height)
    {
        // En un motor AAA, el Resize de recursos masivos se maneja de forma drástica pero segura:
        // Aprovechamos que std::make_unique destruye automáticamente la instancia anterior.
        // Al reasignar, el ComPtr interno del Texture2D liberará la memoria de video antigua, 
        // y el nuevo constructor pedirá la memoria con las nuevas dimensiones.
        
        // Nota: Asegúrate de que tu GraphicsQueue haya hecho un Flush (Wait) de la GPU
        // antes de llamar a este método, para no destruir las texturas mientras la GPU las dibuja.

        Initialize(device, srvHeap, rtvHeap, width, height);
    }

}
