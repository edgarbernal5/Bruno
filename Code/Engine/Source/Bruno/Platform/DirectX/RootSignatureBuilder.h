#pragma once

#include <vector>

#include <wrl.h>
#include <d3d12.h>

namespace Bruno
{
    class RootSignatureBuilder {
    public:
        RootSignatureBuilder& AddDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE rangeType, UINT count, UINT shaderRegister);
        RootSignatureBuilder& AddRootConstantBufferView(UINT shaderRegister, UINT registerSpace = 0, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
        RootSignatureBuilder& AddStaticSampler(UINT shaderRegister);

        // Build: Convierte esto en la estructura nativa de DX12
        Microsoft::WRL::ComPtr<ID3D12RootSignature> Build(ID3D12Device* device) ;

    private:
        std::vector<D3D12_DESCRIPTOR_RANGE> m_ranges;
        std::vector<D3D12_ROOT_PARAMETER> m_parameters;
        std::vector<D3D12_STATIC_SAMPLER_DESC> m_samplers;
    };
}