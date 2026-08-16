#include "brpch.h"
#include "GraphicsPipelineState.h"

#include "RootSignature.h"
#include "GraphicsDevice.h"
#include "Shader.h"
#include "Bruno/Renderer/RHITypes.h"

namespace Bruno
{
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
        
        if (psoDesc.VertexShader != nullptr)
        {
            d3dDesc.VS = psoDesc.VertexShader->GetNativeByteCode();
        }
        
        if (psoDesc.PixelShader != nullptr)
        {
            d3dDesc.PS = psoDesc.PixelShader->GetNativeByteCode();
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

    /*void GraphicsPipelineState::CreateOpaquePSO(ID3D12RootSignature* rootSig, IDxcBlob* vertexShaderByteCode, IDxcBlob* pixelShaderByteCode)
    {
        // 1. Definir el Input Layout (DEBE COINCIDIR CON ModelVertex Y CON EL HLSL)
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature = rootSig;
    
        // 2. Adjuntar los Shaders
        psoDesc.VS = { reinterpret_cast<BYTE*>(vertexShaderByteCode->GetBufferPointer()), vertexShaderByteCode->GetBufferSize() };
        psoDesc.PS = { reinterpret_cast<BYTE*>(pixelShaderByteCode->GetBufferPointer()), pixelShaderByteCode->GetBufferSize() };
    
        // 3. Configurar Estados (Usamos los defaults de d3dx12 para código limpio)
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        // Para ver geometría por dentro y por fuera si no tienes backface culling, usa:
        //psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
        
        // ¡La línea mágica que invierte qué lado es el frente!
        psoDesc.RasterizerState.FrontCounterClockwise = TRUE; 
        psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
        // ... (asignas esto a tu D3D12_GRAPHICS_PIPELINE_STATE_DESC)
        
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // Opaco, sin transparencias
        psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // Z-Buffer activado
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    
        // 4. Formatos de Salida (DEBEN coincidir con tu SwapChain y DepthBuffer)
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // O el que uses en tu SwapChain
        psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;  // O el que uses en tu DepthBuffer
        psoDesc.SampleDesc.Count = 1;
        psoDesc.SampleDesc.Quality = 0;

        // 5. Crear el PSO
        m_device.GetNativeDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pso));
    }*/
}
