#pragma once

#include <d3d12.h>
#include <dxcapi.h>
#include <wrl/client.h>
#include <vector>

#include "Bruno/Renderer/RHITypes.h"

/* TODO:
* Bruno/Renderer/PipelineStateDescriptor.h
namespace Bruno 
{
    
}
void GraphicsPipelineState::Initialize(const PipelineStateDescriptor& desc)
{
    // 1. Aquí dentro traduces Bruno::BlendState a D3D12_BLEND_DESC
    D3D12_GRAPHICS_PIPELINE_STATE_DESC d3d12Desc = {};
    
    // Configuración de Blending basada en tu enum
    d3d12Desc.BlendState = TranslateBlendState(desc.BlendMode);
    
    // Configuración de Depth basada en tu enum
    d3d12Desc.DepthStencilState = TranslateDepthStencilState(desc.DepthMode);
    
    // ... completar el resto con Shaders, InputLayout, etc.
    
    ThrowIfFailed(m_device.GetNativeDevice()->CreateGraphicsPipelineState(
        &d3d12Desc, IID_PPV_ARGS(&m_pso)));
}
*/
namespace Bruno
{
   /* enum class TextureFormat;
    enum class FillMode;
    enum class CullMode;
    enum class VertexFormat;
    enum class DepthMode;
    enum class BlendMode;
    enum class PrimitiveTopology;*/
    
    struct InputElementDesc;
    class ShaderProgram;
    class RootSignature;
    class GraphicsDevice;

    struct GraphicsPipelineStateDesc 
    {
        RootSignature* RootSignature;
        ShaderProgram* VertexShader;
        ShaderProgram* PixelShader;
        
        std::vector<InputElementDesc> InputLayout;
        
        struct {
            CullMode CullMode = CullMode::Back;
            FillMode FillMode = FillMode::Solid;
            bool FrontCounterClockwise = false;
        } RasterizerState;

        struct {
            BlendMode Mode = BlendMode::Opaque;
        } BlendState;

        struct {
            DepthMode Mode = DepthMode::ReadWrite;
            ComparisonFunc Func = ComparisonFunc::Less;
        } DepthState;
        
        PrimitiveTopology Topology;
        
        uint32_t NumRenderTargets = 1;
        
        // DX12 soporta hasta 8 Render Targets simultáneos por defecto
        TextureFormat RTVFormats[8] =
        { 
            TextureFormat::R8G8B8A8_Unorm, // El target 0 por defecto
            TextureFormat::Unknown, TextureFormat::Unknown, TextureFormat::Unknown,
            TextureFormat::Unknown, TextureFormat::Unknown, TextureFormat::Unknown, TextureFormat::Unknown
        };
        
        TextureFormat DSVFormat = TextureFormat::D32_Float; 

        MultiSampleState MultiSample;
        
    };
    
    class GraphicsPipelineState
    {
    public:
        GraphicsPipelineState(GraphicsDevice& device);
        ~GraphicsPipelineState() = default;
        
        [[nodiscard]] ID3D12PipelineState* GetNative() const { return m_pso.Get(); }
        
        // Método 100% genérico. Quien lo llama es el responsable de llenar el descriptor.
        void Initialize(const GraphicsPipelineStateDesc& psoDesc);
        
    private:
        DXGI_FORMAT GetDX12VertexFormat(VertexFormat format);
        D3D12_RASTERIZER_DESC GetDX12RasterizerState(CullMode cull, FillMode fill);
        D3D12_BLEND_DESC GetDX12BlendState(BlendMode mode);
        D3D12_DEPTH_STENCIL_DESC GetDX12DepthState(DepthMode mode, ComparisonFunc func);
        D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveTopologyType(PrimitiveTopology topology);
        DXGI_FORMAT GetDX12Format(TextureFormat format);
        constexpr D3D12_COMPARISON_FUNC GetDX12ComparisonFunc(ComparisonFunc func);
        
        GraphicsDevice& m_device;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pso;
    };

    constexpr D3D12_COMPARISON_FUNC GraphicsPipelineState::GetDX12ComparisonFunc(ComparisonFunc func)
    {
        switch (func)
        {
        case ComparisonFunc::Never:        return D3D12_COMPARISON_FUNC_NEVER;
        case ComparisonFunc::Less:         return D3D12_COMPARISON_FUNC_LESS;
        case ComparisonFunc::Equal:        return D3D12_COMPARISON_FUNC_EQUAL;
        case ComparisonFunc::LessEqual:    return D3D12_COMPARISON_FUNC_LESS_EQUAL;
        case ComparisonFunc::Greater:      return D3D12_COMPARISON_FUNC_GREATER;
        case ComparisonFunc::NotEqual:     return D3D12_COMPARISON_FUNC_NOT_EQUAL;
        case ComparisonFunc::GreaterEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
        case ComparisonFunc::Always:       return D3D12_COMPARISON_FUNC_ALWAYS;
        default:                           return D3D12_COMPARISON_FUNC_LESS;
        }
    }
}
