#include "brpch.h"
#include "RootSignature.h"

#include "GraphicsDevice.h"
#include <stdexcept>

#include "Bruno/Renderer/RHITypes.h"

namespace Bruno
{
    RootSignature::RootSignature(GraphicsDevice& device) :
        m_device(device)
    {
    }

    void RootSignature::AddConstantBufferView(uint32_t shaderRegister, uint32_t registerSpace,
        ShaderVisibility visibility)
    {
    }

    void RootSignature::AddDescriptorTableSRV(uint32_t numDescriptors, uint32_t shaderRegister,
        ShaderVisibility visibility)
    {
    }

    void RootSignature::AddStaticSampler(uint32_t shaderRegister, ShaderVisibility visibility)
    {
    }

    void RootSignature::Build()
    {
    }

    void RootSignature::Initialize(
     UINT numParameters, 
     const CD3DX12_ROOT_PARAMETER* parameters, 
     UINT numSamplers, 
     const CD3DX12_STATIC_SAMPLER_DESC* samplers, 
     D3D12_ROOT_SIGNATURE_FLAGS flags)
    {
        // 1. Ensamblar la estructura base
        CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
            numParameters, parameters,
            numSamplers, samplers,
            flags
        );

        // 2. Serializar a un Blob de memoria
        Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
    
        HRESULT hr = D3D12SerializeRootSignature(
            &rootSigDesc, 
            D3D_ROOT_SIGNATURE_VERSION_1, 
            &serializedRootSig, 
            &errorBlob
        );

        if (FAILED(hr))
        {
            if (errorBlob)
            {
                // Opcional: Imprimir el error del shader (OutputDebugStringA o tu Logger)
                const char* errorMsg = static_cast<const char*>(errorBlob->GetBufferPointer());
                // BR_CORE_ERROR("Root Signature Error: {}", errorMsg);
            }
            throw std::runtime_error("Fallo al serializar la Root Signature");
        }
    
        // 3. Crear el objeto en la GPU
        hr = m_device.GetNativeDevice()->CreateRootSignature(
            0, 
            serializedRootSig->GetBufferPointer(), 
            serializedRootSig->GetBufferSize(), 
            IID_PPV_ARGS(&m_rootSignature)
        );

        if (FAILED(hr))
        {
            throw std::runtime_error("Fallo al crear la Root Signature en el Device");
        }
    }

    D3D12_CULL_MODE RootSignature::GetDX12CullMode(CullMode mode)
    {
        switch(mode) {
        case CullMode::None: return D3D12_CULL_MODE_NONE;
        case CullMode::Front: return D3D12_CULL_MODE_FRONT;
        case CullMode::Back: return D3D12_CULL_MODE_BACK;
        }
        return D3D12_CULL_MODE_BACK;
    }

    /*void RootSignature::CreateOpaqueSignature()
    {
        // 1. Definir los parámetros (El contrato con el shader)
        CD3DX12_ROOT_PARAMETER rootParameters[2];

        // Parámetro 0: Constant Buffer View (CBV) en el registro b0
        // Usamos SHADER_VISIBILITY_VERTEX porque solo el Vertex Shader usa la matriz
        rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

        // Parámetro 1: Tabla de Descriptores para la Textura en el registro t0
        CD3DX12_DESCRIPTOR_RANGE srvTable;
        srvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // 1 textura, registro t0
    
        // Usamos SHADER_VISIBILITY_PIXEL porque solo el Pixel Shader lee la textura
        rootParameters[1].InitAsDescriptorTable(1, &srvTable, D3D12_SHADER_VISIBILITY_PIXEL);

        // 2. Definir el Sampler Estático (s0)
        // Usar un sampler estático es mucho más eficiente que ponerlo en un Descriptor Heap
        CD3DX12_STATIC_SAMPLER_DESC sampler(
            0,                                 // s0
            D3D12_FILTER_MIN_MAG_MIP_LINEAR,   // Filtro bilineal estándar
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,   // WRAP en U
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,   // WRAP en V
            D3D12_TEXTURE_ADDRESS_MODE_WRAP    // WRAP en W
        );

        // 3. Ensamblar la Root Signature
        CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
            2, rootParameters, 
            1, &sampler, 
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT // Requisito vital
        );

        // 4. Serializar y crear (Manejo de errores simplificado)
        Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
    
        D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSig, &errorBlob);
    
        m_device.GetNativeDevice()->CreateRootSignature(
            0, 
            serializedRootSig->GetBufferPointer(), 
            serializedRootSig->GetBufferSize(), 
            IID_PPV_ARGS(&m_rootSignature) // Tu puntero interno de la clase
        );
    }*/
}
