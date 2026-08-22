#include "brpch.h"
#include "GraphicsPipelineState.h"

#include "RootSignature.h"
#include "GraphicsDevice.h"
#include "Shader.h"
#include "Bruno/Renderer/RHITypes.h"
#include "Bruno/Renderer/ShaderLibrary.h"

namespace Bruno
{
    size_t GraphicsPipelineStateDesc::ComputeHash() const
    {
        size_t seed = 0;

        // Puntero del Root Signature 
        // (Es seguro usar la dirección de memoria porque la Library los mantiene vivos siempre)
        HashCombine(seed, reinterpret_cast<size_t>(RootSignature));

        HashCombine(seed, VertexShaderDesc.ComputeHash());
        HashCombine(seed, PixelShaderDesc.ComputeHash());

        HashCombine(seed, static_cast<int>(RasterizerState.CullMode));
        HashCombine(seed, static_cast<int>(RasterizerState.FillMode));
        HashCombine(seed, RasterizerState.FrontCounterClockwise);
        
        HashCombine(seed, static_cast<int>(BlendState.Mode));
        HashCombine(seed, static_cast<int>(DepthState.Mode));
        
        HashCombine(seed, static_cast<int>(Topology));
        
        HashCombine(seed, NumRenderTargets);
        for (uint32_t i = 0; i < NumRenderTargets; ++i)
        {
            HashCombine(seed, static_cast<int>(RTVFormats[i]));
        }
        HashCombine(seed, static_cast<int>(DSVFormat));
        
        HashCombine(seed, MultiSample.Count);
        HashCombine(seed, MultiSample.Quality);
        
        for (const auto& element : InputLayout)
        {
            // std::string_view es excelente para hashear char* rápido
            HashCombine(seed, std::string_view(element.SemanticName)); 
            HashCombine(seed, element.SemanticIndex);
            HashCombine(seed, static_cast<int>(element.Format));
        }

        return seed;
    }

    GraphicsPipelineState::GraphicsPipelineState(GraphicsDevice& device) :
        m_device(device)
    {
    }

    void GraphicsPipelineState::Initialize(const GraphicsPipelineStateDesc& psoDesc)
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dDesc = {};
        
        std::vector<D3D12_INPUT_ELEMENT_DESC> dx12Layout;
        dx12Layout.reserve(psoDesc.InputLayout.size());
        
        for (const auto& element : psoDesc.InputLayout)
        {
            D3D12_INPUT_ELEMENT_DESC dxDesc = {};
            dxDesc.SemanticName = element.SemanticName;
            dxDesc.SemanticIndex = element.SemanticIndex;
            dxDesc.Format = GetDX12VertexFormat(element.Format);
            dxDesc.InputSlot = element.InputSlot;
    
            dxDesc.AlignedByteOffset = (element.AlignedByteOffset == AppendAlignedElement) 
                                        ? D3D12_APPEND_ALIGNED_ELEMENT 
                                        : element.AlignedByteOffset;
                                
            dxDesc.InputSlotClass = (element.Classification == InputClassification::PerVertex)
                                        ? D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA
                                        : D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
                                
            dxDesc.InstanceDataStepRate = element.InstanceStepRate;
    
            dx12Layout.push_back(dxDesc);
        }
        
        d3dDesc.InputLayout = { dx12Layout.data(), static_cast<UINT>(dx12Layout.size()) };
        d3dDesc.pRootSignature = psoDesc.RootSignature->GetNative();
        
        if (psoDesc.VertexShaderDesc)
        {
            auto vertexShader = ShaderLibrary::GetOrCompile(psoDesc.VertexShaderDesc);
            
            d3dDesc.VS = vertexShader->GetNativeByteCode();
        }
        
        if (psoDesc.PixelShaderDesc)
        {
            auto pixelShader = ShaderLibrary::GetOrCompile(psoDesc.PixelShaderDesc);
            
            d3dDesc.PS = pixelShader->GetNativeByteCode();
        }
        
        d3dDesc.RasterizerState = GetDX12RasterizerState(psoDesc.RasterizerState.CullMode , psoDesc.RasterizerState.FillMode);
        d3dDesc.DepthStencilState = GetDX12DepthState(psoDesc.DepthState.Mode, psoDesc.DepthState.Func);
        d3dDesc.BlendState = GetDX12BlendState(psoDesc.BlendState.Mode);
        
        d3dDesc.PrimitiveTopologyType = GetPrimitiveTopologyType(psoDesc.Topology);
        
        d3dDesc.NumRenderTargets = psoDesc.NumRenderTargets;
        for (uint32_t i = 0; i < psoDesc.NumRenderTargets; ++i)
        {
            d3dDesc.RTVFormats[i] = GetDX12Format(psoDesc.RTVFormats[i]);
        }
        d3dDesc.DSVFormat = GetDX12Format(psoDesc.DSVFormat);
        d3dDesc.SampleDesc.Count = psoDesc.MultiSample.Count;
        d3dDesc.SampleDesc.Quality = psoDesc.MultiSample.Quality;
        d3dDesc.SampleMask = UINT_MAX;
        
        // Le pasamos el descriptor directamente a la API gráfica
        HRESULT hr = m_device.GetNativeDevice()->CreateGraphicsPipelineState(
            &d3dDesc, 
            IID_PPV_ARGS(&m_pso)
        );

        if (FAILED(hr))
        {
            // Aquí podrías agregar logs más detallados en el futuro
            throw std::runtime_error("Fallo al crear el Graphics Pipeline State.");
        }
    }

    DXGI_FORMAT GraphicsPipelineState::GetDX12VertexFormat(VertexFormat format)
    {
        switch (format) {
        case VertexFormat::Float:  return DXGI_FORMAT_R32_FLOAT;
        case VertexFormat::Float2: return DXGI_FORMAT_R32G32_FLOAT;
        case VertexFormat::Float3: return DXGI_FORMAT_R32G32B32_FLOAT;
        case VertexFormat::Float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case VertexFormat::Color_RGBA8: return DXGI_FORMAT_R8G8B8A8_UNORM;
        default: return DXGI_FORMAT_UNKNOWN;
        }
    }

    D3D12_RASTERIZER_DESC GraphicsPipelineState::GetDX12RasterizerState(CullMode cull, FillMode fill)
    {
        CD3DX12_RASTERIZER_DESC desc(D3D12_DEFAULT);
        
        desc.FillMode = (fill == FillMode::Wireframe) ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;
        
        switch (cull) {
        case CullMode::None:  desc.CullMode = D3D12_CULL_MODE_NONE; break;
        case CullMode::Front: desc.CullMode = D3D12_CULL_MODE_FRONT; break;
        case CullMode::Back:  desc.CullMode = D3D12_CULL_MODE_BACK; break;
        }
        
        // La línea mágica para modelos importados de Assimp/Blender
        desc.FrontCounterClockwise = TRUE; 
        
        return desc;
    }

    D3D12_BLEND_DESC GraphicsPipelineState::GetDX12BlendState(BlendMode mode)
    {
        // CD3DX12_BLEND_DESC con D3D12_DEFAULT crea un Opaque normal por defecto
        D3D12_BLEND_DESC desc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

        switch (mode)
        {
        case BlendMode::AlphaBlend:
            desc.RenderTarget[0].BlendEnable = TRUE;
            desc.RenderTarget[0].LogicOpEnable = FALSE;
            desc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
            desc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
            desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
            desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
            desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
            desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
            desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
            break;

        case BlendMode::Additive:
            desc.RenderTarget[0].BlendEnable = TRUE;
            desc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
            desc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
            desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
            desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
            desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
            desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
            desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
            break;

        case BlendMode::Opaque:
        default:
            // Ya está configurado como Opaque por CD3DX12_BLEND_DESC
            break;
        }

        // Nota: Si en el futuro soportas múltiples Render Targets con distinto blending,
        // puedes iterar sobre desc.RenderTarget[i] según psoDesc.NumRenderTargets
        return desc;
    }

    D3D12_DEPTH_STENCIL_DESC GraphicsPipelineState::GetDX12DepthState(DepthMode mode, ComparisonFunc func)
    {
        CD3DX12_DEPTH_STENCIL_DESC desc(D3D12_DEFAULT);
        
        desc.DepthFunc = GetDX12ComparisonFunc(func);
        
        switch (mode)
        {
        case DepthMode::None:
            desc.DepthEnable = FALSE;
            break;
        case DepthMode::ReadOnly:
            desc.DepthEnable = TRUE;
            desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
            break;
        case DepthMode::ReadWrite:
            desc.DepthEnable = TRUE;
            desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
            break;
        }
        
        return desc;
    }

    D3D12_PRIMITIVE_TOPOLOGY_TYPE GraphicsPipelineState::GetPrimitiveTopologyType(PrimitiveTopology topology)
    {
        switch (topology)
        {
        case PrimitiveTopology::PointList:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
            
        case PrimitiveTopology::LineList:
        case PrimitiveTopology::LineStrip:
            // Para el PSO, tanto List como Strip se consideran genéricamente "LINE"
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
            
        case PrimitiveTopology::TriangleList:
        case PrimitiveTopology::TriangleStrip:
            // Para el PSO, tanto List como Strip se consideran genéricamente "TRIANGLE"
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            
        default:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
        }
    }

    DXGI_FORMAT GraphicsPipelineState::GetDX12Format(TextureFormat format)
    {
        switch (format) {
        case TextureFormat::R8G8B8A8_Unorm:      return DXGI_FORMAT_R8G8B8A8_UNORM;
        case TextureFormat::R8G8B8A8_Unorm_SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case TextureFormat::B8G8R8A8_Unorm:      return DXGI_FORMAT_B8G8R8A8_UNORM;
        case TextureFormat::R16G16B16A16_Float:  return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case TextureFormat::R32G32B32A32_Float:  return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case TextureFormat::D32_Float:           return DXGI_FORMAT_D32_FLOAT;
        case TextureFormat::D24_Unorm_S8_Uint:   return DXGI_FORMAT_D24_UNORM_S8_UINT;
        default: return DXGI_FORMAT_UNKNOWN;
        }
    }
    
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
