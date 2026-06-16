#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <wrl/client.h>
#include <vector>

namespace Bruno::DX {

    class RootSignature;
    class GraphicsDevice;

    class GraphicsPipelineState {
    public:
        // C++17: Pasamos los blobs compilados y la firma
        GraphicsPipelineState(GraphicsDevice& device);
        
        ~GraphicsPipelineState() = default;

        [[nodiscard]] ID3D12PipelineState* GetNative() const { return m_pso.Get(); }
        void CreateOpaquePSO(ID3D12RootSignature* rootSig, IDxcBlob* vertexShaderByteCode, IDxcBlob* pixelShaderByteCode);
        
    private:
        GraphicsDevice& m_device;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pso;
    };
}