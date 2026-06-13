#pragma once

#include "D3DHelpers.h"
#include <string>
#include <wrl/client.h>
#include <dxcapi.h>

namespace Bruno::DX
{
    class ShaderCompiler {
    public:
        ShaderCompiler();
        ~ShaderCompiler() = default;

        // C++17: Evitamos copias innecesarias usando std::wstring_view
        [[nodiscard]] Microsoft::WRL::ComPtr<IDxcBlob> CompileFromFile(
            std::wstring_view filePath,
            std::wstring_view entryPoint,
            std::wstring_view targetProfile); // ej: L"vs_6_0" o L"ps_6_0"

    private:
        Microsoft::WRL::ComPtr<IDxcUtils> m_utils;
        Microsoft::WRL::ComPtr<IDxcCompiler3> m_compiler;
        Microsoft::WRL::ComPtr<IDxcIncludeHandler> m_includeHandler;
    };

}