#pragma once

#include <cstdint>
#include "Bruno/Core/Base.h"

namespace Bruno
{
    enum class PrimitiveTopology : uint8_t
    {
        PointList,
        LineList,
        LineStrip,
        TriangleList,
        TriangleStrip
    };

    enum class ResourceState : uint8_t
    {
        Common,
        VertexAndConstantBuffer,
        IndexBuffer,
        RenderTarget,
        UnorderedAccess,
        DepthWrite,
        DepthRead,
        NonPixelShaderResource,
        PixelShaderResource,
        CopyDest,
        CopySource,
        Present,
        GenericRead
    };
    
    enum class CullMode : uint8_t
    {
        None, 
        Front, 
        Back
    };
    
    enum class FillMode : uint8_t
    {
        Solid,
        Wireframe
    };
    
    enum class BlendMode : uint8_t
    { 
        Opaque, 
        AlphaBlend, 
        Additive 
    };
    
    
    struct RasterizerState
    {
        CullMode CullMode = CullMode::Back;
        FillMode FillMode = FillMode::Solid;
        int DepthBias = 0;
        float DepthBiasClamp = 0.0f;
        float SlopeScaledDepthBias = 0.0f;
        bool FrontCounterClockwise = true;
    };
    
    enum class ShaderVisibility : uint8_t
    {
        All, 
        Vertex, 
        Pixel, 
        Geometry
    };
    enum class TextureFilter : uint8_t
    {
        Point,          // Pixel art, texturas crudas sin interpolación
        Linear,         // Suavizado estándar (Bilineal/Trilineal)
        Anisotropic,     // Máxima calidad para texturas vistas en ángulo (ej. el suelo)
        Comparison_MinMag_Linear_MipPoint,
    };

    enum class TextureAddressMode : uint8_t
    {
        Wrap,           // Repite la textura infinitamente (Tiling)
        Clamp,          // Estira el último píxel del borde
        Mirror,         // Repite la textura pero invertida como un espejo
        Border          // Pinta un color sólido (ej. blanco o negro) fuera del rango
    };

    enum class DepthMode : uint8_t
    { 
        ReadWrite, // Z-Buffer activado y escribe (Geometría opaca)
        ReadOnly,  // Z-Buffer activado pero no escribe (Partículas, UI transparente)
        None       // Z-Buffer desactivado (UI, Gizmos sobrepuestos)
    };
    enum class ComparisonFunc : uint8_t
    {
        Never,
        Less,
        Equal,
        LessEqual,
        Greater,
        NotEqual,
        GreaterEqual,
        Always
    };
    
    enum class TextureFormat
    {
        Unknown,
        // Formatos de Color estándar
        R8G8B8A8_Unorm,       // Estándar para UI o texturas básicas
        R8G8B8A8_Unorm_SRGB,  // Albedo con corrección Gamma
        B8G8R8A8_Unorm,       // Usado frecuentemente en el SwapChain
        
        // Formatos HDR y Alta Precisión
        R16G16B16A16_Float,   // Perfecto para HDR, Bloom y emisivos
        R32G32B32A32_Float,   // Alta precisión (ej: G-Buffer Positions)
        
        // Formatos de Profundidad (Depth / Stencil)
        D32_Float,            // Profundidad de alta precisión pura
        D24_Unorm_S8_Uint     // Profundidad estándar + Stencil de 8 bits
    };

    // Estructura para el Multi-Sample Anti-Aliasing (MSAA)
    struct MultiSampleState
    {
        uint32_t Count = 1;   // 1 = Sin Anti-Aliasing. (Opciones: 2, 4, 8)
        uint32_t Quality = 0; // Depende del hardware
    };
    
    enum class RootSignatureFlags : uint32_t
    {
        None = 0,
    
        // Indica que esta firma se usará con un Vertex Buffer / Index Buffer.
        // En DX12 esto activará ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT.
        // En Vulkan, simplemente se ignorará (no hace falta).
        AllowInputAssembler = 1 << 0,

        // (Futuro) Podrías agregar cosas como:
        // LocalSignatureForRaytracing = 1 << 1
    };
    BR_DEFINITION_FLAG_FROM_ENUM(RootSignatureFlags);

    struct Rect
    {
        long Left = 0;
        long Top = 0;
        long Right = 0;
        long Bottom = 0;

        explicit operator D3D12_RECT() const noexcept { return *reinterpret_cast<const D3D12_RECT*>(this); }
        const D3D12_RECT* Get12() const noexcept { return reinterpret_cast<const D3D12_RECT*>(this); }
    };
    
    enum class VertexFormat : uint8_t
    {
        Unknown,
        Float,      // 1 float (Ej: un escalar)
        Float2,     // 2 floats (Ej: UVs)
        Float3,     // 3 floats (Ej: Posición, Normales)
        Float4,     // 4 floats (Ej: Color con Alpha)
        Color_RGBA8 // 4 bytes unorm (Color compactado)
    };

    enum class InputClassification : uint8_t
    {
        PerVertex,
        PerInstance
    };

    // Un valor especial equivalente a D3D12_APPEND_ALIGNED_ELEMENT
    constexpr uint32_t AppendAlignedElement = 0xFFFFFFFF;
    
    struct InputElementDesc
    {
        const char* SemanticName;           // Ej: "POSITION", "TEXCOORD"
        uint32_t SemanticIndex;             // Ej: 0, 1, 2
        VertexFormat Format;                // Ej: VertexFormat::Float3
        uint32_t InputSlot;                 // Buffer de entrada (0 normalmente)
        uint32_t AlignedByteOffset;         // Offset en bytes (usamos Append por defecto)
        InputClassification Classification; // Vértice o Instancia
        uint32_t InstanceStepRate;          // 0 para vértices, 1 para instancias

        // Constructor amigable para escribir menos código
        InputElementDesc(
            const char* semanticName,
            uint32_t semanticIndex,
            VertexFormat format,
            uint32_t alignedByteOffset = AppendAlignedElement,
            uint32_t inputSlot = 0,
            InputClassification classification = InputClassification::PerVertex,
            uint32_t instanceStepRate = 0)
            : SemanticName(semanticName)
            , SemanticIndex(semanticIndex)
            , Format(format)
            , InputSlot(inputSlot)
            , AlignedByteOffset(alignedByteOffset)
            , Classification(classification)
            , InstanceStepRate(instanceStepRate)
        {
        }
    };
}
