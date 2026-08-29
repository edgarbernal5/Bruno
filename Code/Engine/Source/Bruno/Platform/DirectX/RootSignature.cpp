#include "brpch.h"
#include "RootSignature.h"

#include "GraphicsDevice.h"
#include <stdexcept>

#include "D3DFunctions.h"
#include "Bruno/Renderer/RHITypes.h"

namespace Bruno
{
    RootSignature::RootSignature(GraphicsDevice& device) :
        m_device(device)
    {
    }

    void RootSignature::AddConstants(uint32_t num32BitValues, uint32_t shaderRegister, uint32_t registerSpace, ShaderVisibility visibility)
    {
        CD3DX12_ROOT_PARAMETER param;
        param.InitAsConstants(num32BitValues, shaderRegister, registerSpace, D3DFunctions::GetDX12Visibility(visibility));
        m_parameters.push_back(param);
        
        // Acumulamos el hash
        HashCombine(m_hash, static_cast<uint32_t>(4)); // 4 = Tipo Constants
        HashCombine(m_hash, num32BitValues);
        HashCombine(m_hash, shaderRegister);
        HashCombine(m_hash, registerSpace);
        HashCombine(m_hash, static_cast<uint32_t>(visibility));
    }

    void RootSignature::AddConstantBufferView(uint32_t shaderRegister, uint32_t registerSpace, ShaderVisibility visibility)
    {
        CD3DX12_ROOT_PARAMETER param;

        param.InitAsConstantBufferView(shaderRegister, registerSpace, D3DFunctions::GetDX12Visibility(visibility));
        m_parameters.push_back(param);
        
        // Acumulamos el hash
        HashCombine(m_hash, static_cast<uint32_t>(1)); // 1 = Tipo CBV
        HashCombine(m_hash, shaderRegister);
        HashCombine(m_hash, registerSpace);
        HashCombine(m_hash, static_cast<uint32_t>(visibility));
    }

    void RootSignature::AddDescriptorTableSRV(uint32_t numDescriptors, uint32_t shaderRegister, uint32_t registerSpace, ShaderVisibility visibility)
    {
        auto range = std::make_unique<CD3DX12_DESCRIPTOR_RANGE[]>(1);
        range[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, numDescriptors, shaderRegister, registerSpace);

        CD3DX12_ROOT_PARAMETER param;
        param.InitAsDescriptorTable(1, range.get(), D3DFunctions::GetDX12Visibility(visibility));

        m_descriptorRanges.push_back(std::move(range));
        m_parameters.push_back(param);
        
        HashCombine(m_hash, static_cast<uint32_t>(2)); // 2 = Tipo SRV Table
        HashCombine(m_hash, numDescriptors);
        HashCombine(m_hash, shaderRegister);
        HashCombine(m_hash, registerSpace);
        HashCombine(m_hash, static_cast<uint32_t>(visibility));
    }

    void RootSignature::AddStaticSampler(uint32_t shaderRegister, uint32_t registerSpace, TextureFilter filter, TextureAddressMode addressMode, ShaderVisibility visibility)
    {
        D3D12_FILTER dxFilter = D3DFunctions::GetDX12Filter(filter);
        D3D12_TEXTURE_ADDRESS_MODE dxAddress = D3DFunctions::GetDX12AddressMode(addressMode);

        // Nota: Si usas Anisotropic, es buena práctica pasar el máximo soportado (16)
        UINT maxAnisotropy = (filter == TextureFilter::Anisotropic) ? 16 : 1;

        CD3DX12_STATIC_SAMPLER_DESC sampler(
            shaderRegister,                    // s#
            dxFilter,                          // Filtro traducido
            dxAddress, dxAddress, dxAddress,   // Address U, V, W traducidos
            0.0f,                              // MipLODBias
            maxAnisotropy,                     // MaxAnisotropy dinámico
            D3D12_COMPARISON_FUNC_LESS_EQUAL,  
            D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, // Color si usas TextureAddressMode::Border
            0.0f,                              // MinLOD
            D3D12_FLOAT32_MAX,                 // MaxLOD
            D3DFunctions::GetDX12Visibility(visibility), 
            registerSpace
        );

        m_staticSamplers.push_back(sampler);
        
        HashCombine(m_hash, static_cast<uint32_t>(3)); // 3 = Tipo Sampler
        HashCombine(m_hash, shaderRegister);
        HashCombine(m_hash, registerSpace);
        HashCombine(m_hash, static_cast<uint32_t>(filter));
        HashCombine(m_hash, static_cast<uint32_t>(addressMode));
        HashCombine(m_hash, static_cast<uint32_t>(visibility));
    }
    
    void RootSignature::Build(RootSignatureFlags flags)
    {
        D3D12_ROOT_SIGNATURE_FLAGS dxFlags = D3DFunctions::GetDX12RootSignatureFlags(flags);

        CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc;
        rootSigDesc.Init(
            static_cast<UINT>(m_parameters.size()),
            m_parameters.data(),
            static_cast<UINT>(m_staticSamplers.size()),
            m_staticSamplers.data(),
            dxFlags // Pasamos los flags traducidos
        );

        Microsoft::WRL::ComPtr<ID3DBlob> serializedSignature;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

        HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedSignature, &errorBlob);

        if (FAILED(hr))
        {
            if (errorBlob)
            {
                const char* errorMsg = static_cast<const char*>(errorBlob->GetBufferPointer());
                // Imprime errorMsg en la consola de tu motor aquí
                OutputDebugStringA(errorMsg); 
            }
            ThrowIfFailed(hr);
        }

        ThrowIfFailed(m_device.GetNativeDevice()->CreateRootSignature(
            0,
            serializedSignature->GetBufferPointer(),
            serializedSignature->GetBufferSize(),
            IID_PPV_ARGS(&m_rootSignature)
        ));
    }

    size_t RootSignature::ComputeHash(RootSignatureFlags flags) const
    {
        size_t finalHash = m_hash;
        HashCombine(finalHash, static_cast<uint32_t>(flags));
        return finalHash;
    }
}
