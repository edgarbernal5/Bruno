#pragma once

#include "D3DHelpers.h"
#include "Device.h"
#include <d3d12.h>
#include <wrl/client.h>

namespace Bruno::DX
{
    class RootSignature {
    public:
        // Constructor que arma nuestra firma específica para 3D Texturizado
        explicit RootSignature(GraphicsDevice& device);
        ~RootSignature() = default;

        [[nodiscard]] ID3D12RootSignature* GetNative() const { return m_rootSignature.Get(); }

    private:
        Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
    };

}