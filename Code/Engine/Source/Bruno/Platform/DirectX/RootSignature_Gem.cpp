#include "brpch.h"
#include "RootSignature_Gem.h"

#include <stdexcept>

namespace Bruno::DX
{
    RootSignature::RootSignature(GraphicsDevice& device)
    {
        auto nativeDevice = device.GetNativeDevice();

        // ---------------------------------------------------------
        // 1. Definir los Parámetros (Lo que recibe el Shader)
        // ---------------------------------------------------------
        
        // Nuestro shader tiene 2 parámetros principales (CBV y Textura)
        CD3DX12_ROOT_PARAMETER1 rootParameters[2];

        // Parámetro 0: Constant Buffer (MVP matrix) -> register(b0)
        // Lo pasamos como un "Root CBV", que es rapidísimo y no requiere Descriptor Heap.
        rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);

        // Parámetro 1: Textura -> register(t0)
        // Las texturas deben pasarse mediante una "Descriptor Table" (Tabla de descriptores)
        CD3DX12_DESCRIPTOR_RANGE1 srvRange;
        srvRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        
        rootParameters[1].InitAsDescriptorTable(1, &srvRange, D3D12_SHADER_VISIBILITY_PIXEL);

        // ---------------------------------------------------------
        // 2. Definir el Static Sampler -> register(s0)
        // ---------------------------------------------------------
        // El Sampler le dice a la GPU cómo filtrar la textura si se estira o se encoge.
        // Un "Static Sampler" se compila directamente en la Root Signature, ahorrando memoria y tiempo.
        CD3DX12_STATIC_SAMPLER_DESC sampler = {};
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // Interpolación suave
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // Repetir textura en U
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // Repetir textura en V
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.MipLODBias = 0;
        sampler.MaxAnisotropy = 0;
        sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        sampler.MinLOD = 0.0f;
        sampler.MaxLOD = D3D12_FLOAT32_MAX;
        sampler.ShaderRegister = 0; // register(s0)
        sampler.RegisterSpace = 0;
        sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        // ---------------------------------------------------------
        // 3. Armar y Serializar la Root Signature
        // ---------------------------------------------------------
        
        // ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT es obligatorio para poder usar Vertex Buffers
        D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
                                           D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
                                           D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
                                           D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, flags);

        Microsoft::WRL::ComPtr<ID3DBlob> signature;
        Microsoft::WRL::ComPtr<ID3DBlob> error;

        // "Serializar" significa convertir nuestras estructuras de C++ al código binario que la GPU entiende
        HRESULT hr = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_1, &signature, &error);
        if (FAILED(hr)) {
            if (error) {
                throw std::runtime_error(static_cast<char*>(error->GetBufferPointer()));
            }
            throw std::runtime_error("Fallo al serializar la Root Signature.");
        }

        // 4. Crear el objeto final en la GPU
        hr = nativeDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
        if (FAILED(hr)) {
            throw std::runtime_error("Fallo al crear la Root Signature en el Device.");
        }
    }

    void RootSignature::CreateOpaqueSignature(ID3D12Device* device)
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
    
        device->CreateRootSignature(
            0, 
            serializedRootSig->GetBufferPointer(), 
            serializedRootSig->GetBufferSize(), 
            IID_PPV_ARGS(&m_rootSignature) // Tu puntero interno de la clase
        );
    }
}
