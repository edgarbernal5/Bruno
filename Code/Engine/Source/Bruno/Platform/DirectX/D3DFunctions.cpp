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

    D3D12_RASTERIZER_DESC GetDX12RasterizerState(const RasterizerState& rasterizerState)
    {
        CD3DX12_RASTERIZER_DESC d3dDesc(D3D12_DEFAULT);
        
        d3dDesc.FillMode = (rasterizerState.FillMode == FillMode::Wireframe) ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;
        d3dDesc.DepthBias = rasterizerState.DepthBias;
        d3dDesc.DepthBiasClamp = rasterizerState.DepthBiasClamp;
        d3dDesc.SlopeScaledDepthBias = rasterizerState.SlopeScaledDepthBias;
        d3dDesc.FrontCounterClockwise = rasterizerState.FrontCounterClockwise;
        
        switch (rasterizerState.CullMode)
        {
        case CullMode::None:  d3dDesc.CullMode = D3D12_CULL_MODE_NONE; break;
        case CullMode::Front: d3dDesc.CullMode = D3D12_CULL_MODE_FRONT; break;
        case CullMode::Back:  d3dDesc.CullMode = D3D12_CULL_MODE_BACK; break;
        }
        
        return d3dDesc;
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

    D3D12_FILTER GetDX12Filter(TextureFilter filter)
    {
        switch (filter)
        {
        case TextureFilter::Point:       return D3D12_FILTER_MIN_MAG_MIP_POINT;
        case TextureFilter::Linear:      return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        case TextureFilter::Anisotropic: return D3D12_FILTER_ANISOTROPIC;
        case TextureFilter::Comparison_MinMag_Linear_MipPoint: return D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
        default:                         return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        }
    }
    
    D3D12_CULL_MODE GetDX12CullMode(CullMode mode)
    {
        switch(mode)
        {
        case CullMode::None: return D3D12_CULL_MODE_NONE;
        case CullMode::Front: return D3D12_CULL_MODE_FRONT;
        case CullMode::Back: return D3D12_CULL_MODE_BACK;
        }
        return D3D12_CULL_MODE_BACK;
    }

    D3D12_SHADER_VISIBILITY GetDX12Visibility(ShaderVisibility visibility)
    {
        switch (visibility)
        {
        case ShaderVisibility::Vertex:   return D3D12_SHADER_VISIBILITY_VERTEX;
        case ShaderVisibility::Pixel:    return D3D12_SHADER_VISIBILITY_PIXEL;
        case ShaderVisibility::Geometry: return D3D12_SHADER_VISIBILITY_GEOMETRY;
        case ShaderVisibility::All:      
        default:                         return D3D12_SHADER_VISIBILITY_ALL;
        }
    }

    D3D12_TEXTURE_ADDRESS_MODE GetDX12AddressMode(TextureAddressMode mode)
    {
        switch (mode)
        {
        case TextureAddressMode::Wrap:   return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        case TextureAddressMode::Clamp:  return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        case TextureAddressMode::Mirror: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        case TextureAddressMode::Border: return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        default:                         return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        }
    }

    D3D12_ROOT_SIGNATURE_FLAGS GetDX12RootSignatureFlags(RootSignatureFlags flags)
    {
        D3D12_ROOT_SIGNATURE_FLAGS dxFlags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

        if (static_cast<uint32_t>(flags) & static_cast<uint32_t>(RootSignatureFlags::AllowInputAssembler))
        {
            dxFlags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        }

        // Pro-Tip de Optimización AAA:
        // En DX12, es buena práctica denegar el acceso a los shaders que no usas 
        // para que la GPU corra más rápido. Podrías implementar lógica aquí para 
        // denegar Geometry, Hull y Domain shaders por defecto, a menos que tu motor los use.
    
        // dxFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
        // dxFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
        // dxFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

        return dxFlags;
    }

    D3D12_RESOURCE_STATES GetDX12ResourceState(ResourceState state)
    {
        switch (state)
        {
        case ResourceState::Common:                   return D3D12_RESOURCE_STATE_COMMON;
        case ResourceState::VertexAndConstantBuffer:  return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
        case ResourceState::IndexBuffer:              return D3D12_RESOURCE_STATE_INDEX_BUFFER;
        case ResourceState::RenderTarget:             return D3D12_RESOURCE_STATE_RENDER_TARGET;
        case ResourceState::UnorderedAccess:          return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        case ResourceState::DepthWrite:               return D3D12_RESOURCE_STATE_DEPTH_WRITE;
        case ResourceState::DepthRead:                return D3D12_RESOURCE_STATE_DEPTH_READ;
        case ResourceState::NonPixelShaderResource:   return D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        case ResourceState::PixelShaderResource:      return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        case ResourceState::CopyDest:                 return D3D12_RESOURCE_STATE_COPY_DEST;
        case ResourceState::CopySource:               return D3D12_RESOURCE_STATE_COPY_SOURCE;
        case ResourceState::Present:                  return D3D12_RESOURCE_STATE_PRESENT;
        case ResourceState::GenericRead:              return D3D12_RESOURCE_STATE_GENERIC_READ;
        default:                                      return D3D12_RESOURCE_STATE_COMMON;
        }
    }
}
