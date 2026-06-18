#pragma once

#include "D3DHelpers.h"
#include <dxcapi.h>

namespace Bruno::DX
{
    enum class ShaderStage 
    {
        Vertex,
        Pixel,
        Compute
    };

    class ShaderProgram 
    {
    public:
        ShaderProgram(ShaderStage stage, Microsoft::WRL::ComPtr<IDxcBlob> byteCode)
            : m_stage(stage), m_byteCode(std::move(byteCode)) {}

        ShaderStage GetStage() const { return m_stage; }
        IDxcBlob* GetByteCode() const { return m_byteCode.Get(); }
    
        // Helper para pasarlo al GraphicsPipelineState directamente
        D3D12_SHADER_BYTECODE GetNativeByteCode() const 
        {
            if (!m_byteCode) return { nullptr, 0 };
            return { m_byteCode->GetBufferPointer(), m_byteCode->GetBufferSize() };
        }

    private:
        ShaderStage m_stage;
        Microsoft::WRL::ComPtr<IDxcBlob> m_byteCode;
    };
    
    class Shader : public Asset // Puede ser un Asset administrado por tu AssetManager
    {
    public:
        Shader(const std::string& name) : m_name(name) {}

        void AddProgram(ShaderProgram&& program);

        const ShaderProgram* GetVertexProgram() const { return m_vertexProgram.get(); }
        const ShaderProgram* GetPixelProgram() const { return m_pixelProgram.get(); }

    private:
        std::string m_name;
        std::unique_ptr<ShaderProgram> m_vertexProgram;
        std::unique_ptr<ShaderProgram> m_pixelProgram;
    };
    
    /*
    // 1. Usar tu compilador para armar el Asset 'Shader'
auto opaqueShader = std::make_shared<Shader>("OpaqueUnlit");

opaqueShader->AddProgram(ShaderProgram(
    ShaderStage::Vertex, 
    compiler.CompileFromFile(L"Shaders/Opaque.hlsl", L"VS", L"vs_6_0")
));

opaqueShader->AddProgram(ShaderProgram(
    ShaderStage::Pixel, 
    compiler.CompileFromFile(L"Shaders/Opaque.hlsl", L"PS", L"ps_6_0")
));

// 2. Le pasas el Objeto 'Shader' limpio a tu PSO
m_opaquePSO->CreateOpaquePSO(
    device, 
    m_opaqueRootSignature->GetNative(), 
    opaqueShader.get() // <--- Pasamos el objeto completo
);
     */
}