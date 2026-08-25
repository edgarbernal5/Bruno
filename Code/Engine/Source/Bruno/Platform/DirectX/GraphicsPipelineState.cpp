#include "brpch.h"
#include "GraphicsPipelineState.h"

#include "D3DFunctions.h"
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
            dxDesc.Format = D3DFunctions::GetDX12VertexFormat(element.Format);
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
        
        d3dDesc.RasterizerState = D3DFunctions::GetDX12RasterizerState(psoDesc.RasterizerState.CullMode , psoDesc.RasterizerState.FillMode);
        d3dDesc.DepthStencilState = D3DFunctions::GetDX12DepthState(psoDesc.DepthState.Mode, psoDesc.DepthState.Func);
        d3dDesc.BlendState = D3DFunctions::GetDX12BlendState(psoDesc.BlendState.Mode);
        
        d3dDesc.PrimitiveTopologyType = D3DFunctions::GetPrimitiveTopologyType(psoDesc.Topology);
        
        d3dDesc.NumRenderTargets = psoDesc.NumRenderTargets;
        for (uint32_t i = 0; i < psoDesc.NumRenderTargets; ++i)
        {
            d3dDesc.RTVFormats[i] = D3DFunctions::GetDX12Format(psoDesc.RTVFormats[i]);
        }
        d3dDesc.DSVFormat = D3DFunctions::GetDX12Format(psoDesc.DSVFormat);
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
}
