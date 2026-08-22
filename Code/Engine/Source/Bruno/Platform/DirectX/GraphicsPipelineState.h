#pragma once

#include <d3d12.h>
#include <dxcapi.h>
#include <wrl/client.h>
#include <vector>

#include "Shader.h"
#include "Bruno/Renderer/RHITypes.h"

namespace Bruno
{
    struct InputElementDesc;
    class RootSignature;
    class GraphicsDevice;

    struct GraphicsPipelineStateDesc 
    {
        RootSignature* RootSignature;
        
        ShaderCompileDesc VertexShaderDesc;
        ShaderCompileDesc PixelShaderDesc;
        
        std::wstring VertexShaderPath;
        std::wstring PixelShaderPath;
        
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
        size_t ComputeHash() const;
        
    };
    
    class GraphicsPipelineState
    {
    public:
        GraphicsPipelineState(GraphicsDevice& device);
        ~GraphicsPipelineState() = default;
        
        [[nodiscard]] ID3D12PipelineState* GetNative() const { return m_pso.Get(); }
        
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
}
