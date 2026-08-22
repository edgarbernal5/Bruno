#include "brpch.h"
#include "ShaderLibrary.h"

#include "Bruno/Platform/DirectX/Shader.h"
#include "Bruno/Platform/DirectX/ShaderCompiler.h"

namespace Bruno
{
    std::unordered_map<size_t, std::shared_ptr<ShaderProgram>> ShaderLibrary::g_shaderMap;
    std::mutex ShaderLibrary::g_mutex;

    std::shared_ptr<ShaderProgram> ShaderLibrary::GetOrCompile(const ShaderCompileDesc& desc)
    {
        size_t hash = desc.ComputeHash();

        std::lock_guard<std::mutex> lock(g_mutex);

        auto it = g_shaderMap.find(hash);
        if (it != g_shaderMap.end())
        {
            return it->second;
        }

        ShaderCompiler compiler;
        auto byteCode = compiler.CompileFromFile(
            desc.FilePath.c_str(),
            desc.EntryPoint.c_str(),
            desc.Profile.c_str()
        );

        if (!byteCode)
        {
            throw std::runtime_error("Fallo al compilar el shader");
        }

        auto newShader = std::make_shared<ShaderProgram>(desc.Profile, byteCode);
        
        g_shaderMap[hash] = newShader;

        return newShader;
    }

    void ShaderLibrary::Clear()
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_shaderMap.clear();
    }
}
