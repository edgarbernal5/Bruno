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
}
