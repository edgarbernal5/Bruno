#include "brpch.h"
#include "GraphicsPipelineState.h"

#include "RootSignature_Gem.h"

namespace Bruno::DX
{
    GraphicsPipelineState::GraphicsPipelineState(ID3D12Device* device, 
                                                 RootSignature* rootSignature,
                                                 IDxcBlob* vertexShader,
                                                 IDxcBlob* pixelShader,
                                                 DXGI_FORMAT renderTargetFormat) 
    {
        // 1. Input Layout (Perfecto para mallas texturizadas)
        std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputLayout.data(), static_cast<UINT>(inputLayout.size()) };
        psoDesc.pRootSignature = rootSignature->GetNative();
        
        // 2. Inyectar los Shaders extraídos del IDxcBlob
        psoDesc.VS = { vertexShader->GetBufferPointer(), vertexShader->GetBufferSize() };
        psoDesc.PS = { pixelShader->GetBufferPointer(), pixelShader->GetBufferSize() };

        // 3. Rasterizador (Solid, Back Culling)
        psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
        psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
        psoDesc.RasterizerState.FrontCounterClockwise = FALSE;
        psoDesc.RasterizerState.DepthClipEnable = TRUE;

        // 4. Blend State (Sin transparencia por ahora)
        psoDesc.BlendState.AlphaToCoverageEnable = FALSE;
        psoDesc.BlendState.IndependentBlendEnable = FALSE;
        psoDesc.BlendState.RenderTarget[0].BlendEnable = FALSE;
        psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

        // 5. Configuración de Profundidad (AJUSTADO PARA 3D)
        // Nota: Asumimos que tienes o tendrás un buffer de profundidad (DSV).
        // Si tu SwapChain no tiene Depth Buffer todavía, puedes dejar DepthEnable = FALSE temporalmente.
        psoDesc.DepthStencilState.DepthEnable = TRUE;
        psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
        psoDesc.DepthStencilState.StencilEnable = FALSE;

        // 6. Topología y formato de salida
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = renderTargetFormat; 
        
        // Formato del Depth Buffer (Ajusta este formato si usas otro, ej. D32_FLOAT)
        psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT; 
        
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.SampleDesc.Count = 1;

        // 7. Compilar y validar el PSO
        HRESULT hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pso));
        if (FAILED(hr))
        {
            // Reemplaza esto con el sistema de logging de tu engine (ej. BRUNO_CORE_ERROR)
            throw std::runtime_error("Fallo al crear el Pipeline State Object (PSO). Revisa que los Shaders hagan match con el Root Signature.");
        }
    }

    void GraphicsPipelineState::CreateOpaquePSO(ID3D12Device* device, ID3D12RootSignature* rootSig, ID3DBlob* vertexShaderByteCode, ID3DBlob* pixelShaderByteCode)
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
        // psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; 
    
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
        device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pso));
    }
}
