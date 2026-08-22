#pragma once

#include "D3DHelpers.h"
#include <dxcapi.h>
#include "Bruno/Content/Asset.h"

namespace Bruno
{
    enum class ShaderStage 
    {
        Vertex,
        Pixel,
        Compute
    };
    
    struct ShaderCompileDesc
    {
        std::wstring FilePath;
        std::wstring EntryPoint;
        std::wstring Profile; // "vs_6_0" o "ps_6_0"

        operator bool() const;
        
        size_t ComputeHash() const;

        bool operator==(const ShaderCompileDesc& other) const
        {
            return FilePath == other.FilePath && 
                   EntryPoint == other.EntryPoint && 
                   Profile == other.Profile;
        }
    };

    class ShaderProgram 
    {
    public:
        ShaderProgram(ShaderStage stage, Microsoft::WRL::ComPtr<IDxcBlob> byteCode)
            : m_stage(stage), m_byteCode(std::move(byteCode))
        {
        }
        ShaderProgram(const std::wstring& profile, Microsoft::WRL::ComPtr<IDxcBlob> byteCode);

        ShaderStage GetStage() const { return m_stage; }
        IDxcBlob* GetByteCode() const { return m_byteCode.Get(); }
    
        // Helper para pasarlo al GraphicsPipelineState directamente
        D3D12_SHADER_BYTECODE GetNativeByteCode() const 
        {
            if (!m_byteCode)
            {
                return { nullptr, 0 };
            }
            
            return { m_byteCode->GetBufferPointer(), m_byteCode->GetBufferSize() };
        }

    private:
        ShaderStage m_stage;
        Microsoft::WRL::ComPtr<IDxcBlob> m_byteCode;
    };
    
    class Shader : public Asset // Puede ser un Asset administrado por tu AssetManager
    {
        BR_RTTI_DECLARATION(Shader, Asset);
        
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
}