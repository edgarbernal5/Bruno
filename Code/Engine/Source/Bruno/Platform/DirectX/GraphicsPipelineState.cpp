#include "brpch.h"
#include "GraphicsPipelineState.h"

#include "RootSignature.h"
#include "GraphicsDevice.h"

namespace Bruno
{
    GraphicsPipelineState::GraphicsPipelineState(GraphicsDevice& device) :
        m_device(device)
    {
    }

    void GraphicsPipelineState::Initialize(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc)
    {
        // Le pasamos el descriptor directamente a la API gráfica
        HRESULT hr = m_device.GetNativeDevice()->CreateGraphicsPipelineState(
            &psoDesc, 
            IID_PPV_ARGS(&m_pso)
        );

        if (FAILED(hr))
        {
            // Aquí podrías agregar logs más detallados en el futuro
            throw std::runtime_error("Fallo al crear el Graphics Pipeline State.");
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
