#include "brpch.h"
#include "ShaderCompiler.h"

namespace Bruno
{
    ShaderCompiler::ShaderCompiler()
    {
        // Inicializamos las fábricas de DXC
        DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&m_utils));
        DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_compiler));
        m_utils->CreateDefaultIncludeHandler(&m_includeHandler);
    }

    Microsoft::WRL::ComPtr<IDxcBlob> ShaderCompiler::CompileFromFile(std::wstring_view filePath, std::wstring_view entryPoint, std::wstring_view targetProfile) 
    {
        // 1. Cargar el archivo fuente
        Microsoft::WRL::ComPtr<IDxcBlobEncoding> sourceBlob;
        HRESULT hr = m_utils->LoadFile(filePath.data(), nullptr, &sourceBlob);
        if (FAILED(hr))
        {
            throw std::runtime_error("Fallo al abrir el archivo del shader.");
        }

        // 2. Configurar los argumentos del compilador (DXC)
        std::vector<LPCWSTR> arguments = {
            filePath.data(),            // Nombre del archivo
            L"-E", entryPoint.data(),   // Entry point (ej: main)
            L"-T", targetProfile.data(),// Target profile (ej: vs_6_0)
            L"-Zi",                     // Habilitar info de debug
            L"-Qstrip_reflect",         // Optimización: quitar reflection del blob principal
            L"-Od"                      // Deshabilitar optimizaciones (útil en Debug)
        };

        DxcBuffer sourceBuffer;
        sourceBuffer.Ptr = sourceBlob->GetBufferPointer();
        sourceBuffer.Size = sourceBlob->GetBufferSize();
        sourceBuffer.Encoding = DXC_CP_ACP; 

        // 3. Compilar
        Microsoft::WRL::ComPtr<IDxcResult> results;
        m_compiler->Compile(
            &sourceBuffer, 
            arguments.data(), 
            static_cast<UINT32>(arguments.size()), 
            m_includeHandler.Get(), 
            IID_PPV_ARGS(&results)
        );

        // 4. Revisar Errores
        Microsoft::WRL::ComPtr<IDxcBlobUtf8> errors = nullptr;
        results->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);
        if (errors != nullptr && errors->GetStringLength() != 0)
        {
            std::cerr << "Shader Compilation Error:\n" << errors->GetStringPointer() << std::endl;
        }

        // 5. Verificar si falló internamente
        results->GetStatus(&hr);
        if (FAILED(hr))
        {
            throw std::runtime_error("El shader falló al compilar.");
        }

        // 6. Extraer el código binario compilado (DXIL)
        Microsoft::WRL::ComPtr<IDxcBlob> shaderCode;
        results->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderCode), nullptr);

        return shaderCode;
    }
}
