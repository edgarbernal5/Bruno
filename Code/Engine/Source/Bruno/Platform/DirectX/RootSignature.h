#pragma once

#include "D3DHelpers.h"
#include <d3d12.h>
#include "d3dx12.h"
#include <wrl/client.h>

namespace Bruno
{
    class GraphicsDevice;

    class RootSignature
    {
    public:
        // Constructor que arma nuestra firma específica para 3D Texturizado
        explicit RootSignature(GraphicsDevice& device);
        ~RootSignature() = default;
        
        void Initialize(
            UINT numParameters, 
            const CD3DX12_ROOT_PARAMETER* parameters,
            UINT numSamplers = 0,
            const CD3DX12_STATIC_SAMPLER_DESC* samplers = nullptr,
            D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
        );
        
        [[nodiscard]] ID3D12RootSignature* GetNative() const { return m_rootSignature.Get(); }
        
    private:
        GraphicsDevice& m_device;
        Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
    };
}