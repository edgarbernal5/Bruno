#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <wrl/client.h>
#include <vector>

/* TODO:
* Bruno/Renderer/PipelineStateDescriptor.h
namespace Bruno 
{
    struct PipelineStateDescriptor 
    {
        Shader* VertexShader;
        Shader* PixelShader;
        BlendState BlendMode;         // Tu propia enum: Opaque, Alpha, Additive
        DepthStencilState DepthMode;  // Tu propia enum: Default, NoDepth, ReadOnly
        PrimitiveTopology Topology;
        // ... otros estados comunes
    };
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
namespace Bruno::DX
{
    class RootSignature;
    class GraphicsDevice;

    class GraphicsPipelineState
    {
    public:
        // C++17: Pasamos los blobs compilados y la firma
        GraphicsPipelineState(GraphicsDevice& device);
        
        ~GraphicsPipelineState() = default;
        
        [[nodiscard]] ID3D12PipelineState* GetNative() const { return m_pso.Get(); }
        
        // Método 100% genérico. Quien lo llama es el responsable de llenar el descriptor.
        void Initialize(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc);
    private:
        GraphicsDevice& m_device;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pso;
    };
}