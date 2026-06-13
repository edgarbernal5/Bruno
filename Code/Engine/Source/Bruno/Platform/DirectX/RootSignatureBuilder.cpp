#include "brpch.h"
#include "RootSignatureBuilder.h"

namespace Bruno::DX
{
    RootSignatureBuilder& RootSignatureBuilder::AddDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE rangeType, UINT count,
        UINT shaderRegister)
    {
        // Configuramos el rango (ej. t0 para texturas)
        D3D12_DESCRIPTOR_RANGE range = {};
        range.RangeType = rangeType;
        range.NumDescriptors = count;
        range.BaseShaderRegister = shaderRegister;
        m_ranges.push_back(range);

        // Configuramos la tabla
        D3D12_ROOT_DESCRIPTOR_TABLE table = {};
        table.NumDescriptorRanges = 1;
        table.pDescriptorRanges = &m_ranges.back();

        D3D12_ROOT_PARAMETER param = {};
        param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        param.DescriptorTable = table;
        param.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        m_parameters.push_back(param);
        
        return *this;
    }

    RootSignatureBuilder& RootSignatureBuilder::AddRootConstantBufferView(UINT shaderRegister, UINT registerSpace, D3D12_SHADER_VISIBILITY visibility)
    {
        D3D12_ROOT_PARAMETER param = {};
        param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        
        // Configuramos el descriptor directo
        param.Descriptor.ShaderRegister = shaderRegister;
        param.Descriptor.RegisterSpace = registerSpace;
        
        // La visibilidad restringe qué etapa del pipeline puede ver esto (optimización AAA)
        param.ShaderVisibility = visibility;

        m_parameters.push_back(param);
        
        return *this;
    }

    RootSignatureBuilder& RootSignatureBuilder::AddStaticSampler(UINT shaderRegister)
    {
        D3D12_STATIC_SAMPLER_DESC sampler = {};
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.ShaderRegister = shaderRegister;
        sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        m_samplers.push_back(sampler);
        
        return *this;
    }

    Microsoft::WRL::ComPtr<ID3D12RootSignature> RootSignatureBuilder::Build(ID3D12Device* device)
    {
        D3D12_ROOT_SIGNATURE_DESC desc = {};
        desc.NumParameters = static_cast<UINT>(m_parameters.size());
        desc.pParameters = m_parameters.data();
        desc.NumStaticSamplers = static_cast<UINT>(m_samplers.size());
        desc.pStaticSamplers = m_samplers.data();
        desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        // Serializamos
        Microsoft::WRL::ComPtr<ID3DBlob> signature, error;
        D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
        
        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSig;
        device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSig));
        return rootSig;
    }
}
