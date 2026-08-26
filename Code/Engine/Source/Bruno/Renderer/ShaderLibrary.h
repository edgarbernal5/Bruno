#pragma once

namespace Bruno
{
    struct ShaderCompileDesc;
    class ShaderProgram;

    class ShaderLibrary
    {
    public:
        // Devuelve un shader compilado. Si no existe, lo compila desde cero.
        static std::shared_ptr<ShaderProgram> GetOrCompile(const ShaderCompileDesc& desc);

        // Opcional: Pre-compilar shaders en una pantalla de carga
        //static void PreloadShader(const ShaderCompileDesc& desc);

        static void Clear();

    private:
        static std::unordered_map<size_t, std::shared_ptr<ShaderProgram>> g_shaderMap;
        static std::mutex g_mutex;
    };
}