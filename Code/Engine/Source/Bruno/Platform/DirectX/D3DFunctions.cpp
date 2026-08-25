#include "brpch.h"
#include "D3DFunctions.h"

namespace Bruno::D3DFunctions
{
    DXGI_FORMAT GetDX12VertexFormat(VertexFormat format)
    {
        switch (format)
        {
        case VertexFormat::Float:  return DXGI_FORMAT_R32_FLOAT;
        case VertexFormat::Float2: return DXGI_FORMAT_R32G32_FLOAT;
        case VertexFormat::Float3: return DXGI_FORMAT_R32G32B32_FLOAT;
        case VertexFormat::Float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case VertexFormat::Color_RGBA8: return DXGI_FORMAT_R8G8B8A8_UNORM;
        default: return DXGI_FORMAT_UNKNOWN;
        }
    }

    D3D12_RASTERIZER_DESC GetDX12RasterizerState(CullMode cull, FillMode fill)
    {
        CD3DX12_RASTERIZER_DESC desc(D3D12_DEFAULT);
        
        desc.FillMode = (fill == FillMode::Wireframe) ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;
        
        switch (cull)
        {
        case CullMode::None:  desc.CullMode = D3D12_CULL_MODE_NONE; break;
        case CullMode::Front: desc.CullMode = D3D12_CULL_MODE_FRONT; break;
        case CullMode::Back:  desc.CullMode = D3D12_CULL_MODE_BACK; break;
        }
        
        // La línea mágica para modelos importados de Assimp/Blender
        desc.FrontCounterClockwise = TRUE; 
        
        return desc;
    }

    D3D12_BLEND_DESC GetDX12BlendState(BlendMode mode)
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

    D3D12_DEPTH_STENCIL_DESC GetDX12DepthState(DepthMode mode, ComparisonFunc func)
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

    D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveTopologyType(PrimitiveTopology topology)
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

    DXGI_FORMAT GetDX12Format(TextureFormat format)
    {
        switch (format)
        {
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
    
    constexpr D3D12_COMPARISON_FUNC GetDX12ComparisonFunc(ComparisonFunc func)
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