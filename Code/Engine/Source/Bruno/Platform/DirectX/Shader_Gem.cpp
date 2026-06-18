#include "brpch.h"
#include "Shader_Gem.h"

namespace Bruno::DX
{
    void Shader::AddProgram(ShaderProgram&& program)
    {
        // En un motor real, podrías usar un map: m_programs[program.GetStage()] = program;
        // o guardarlos en punteros directos para un acceso O(1):
        if (program.GetStage() == ShaderStage::Vertex) m_vertexProgram = std::make_unique<ShaderProgram>(std::move(program));
        else if (program.GetStage() == ShaderStage::Pixel) m_pixelProgram = std::make_unique<ShaderProgram>(std::move(program));
    }
}
