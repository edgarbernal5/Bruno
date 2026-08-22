#include "brpch.h"
#include "Shader.h"

namespace Bruno
{
    BR_RTTI_DEFINITIONS(Shader);

    ShaderCompileDesc::operator bool() const
    {
        return !FilePath.empty() && !EntryPoint.empty() && !Profile.empty();
    }

    size_t ShaderCompileDesc::ComputeHash() const
    {
        size_t seed = 0;
        HashCombine(seed, FilePath);
        HashCombine(seed, EntryPoint);
        HashCombine(seed, Profile);
        return seed;
    }

    ShaderProgram::ShaderProgram(const std::wstring& profile, Microsoft::WRL::ComPtr<IDxcBlob> byteCode) :
        m_byteCode(std::move(byteCode))
    {
        if (profile._Starts_with(L"vs_"))
        {
            m_stage = ShaderStage::Vertex;
        }
        else if (profile._Starts_with(L"ps_"))
        {
            m_stage = ShaderStage::Pixel;
        }
    }
    
    void Shader::AddProgram(ShaderProgram&& program)
    {
        // En un motor real, podrías usar un map: m_programs[program.GetStage()] = program;
        // o guardarlos en punteros directos para un acceso O(1):
        if (program.GetStage() == ShaderStage::Vertex) m_vertexProgram = std::make_unique<ShaderProgram>(std::move(program));
        else if (program.GetStage() == ShaderStage::Pixel) m_pixelProgram = std::make_unique<ShaderProgram>(std::move(program));
    }
}
