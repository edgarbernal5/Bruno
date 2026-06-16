#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <wrl/client.h>
#include <vector>

namespace Bruno::DX {

    class RootSignature;

    class GraphicsPipelineState {
    public:
        // C++17: Pasamos los blobs compilados y la firma
        GraphicsPipelineState(ID3D12Device* device, 
                              RootSignature* rootSignature,
                              IDxcBlob* vertexShader,
                              IDxcBlob* pixelShader,
                              DXGI_FORMAT renderTargetFormat);
        
        ~GraphicsPipelineState() = default;

        [[nodiscard]] ID3D12PipelineState* GetNative() const { return m_pso.Get(); }
        void CreateOpaquePSO(ID3D12Device* device, ID3D12RootSignature* rootSig, ID3DBlob* vertexShaderByteCode, ID3DBlob* pixelShaderByteCode);
        
    private:
        Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pso;
    };
}