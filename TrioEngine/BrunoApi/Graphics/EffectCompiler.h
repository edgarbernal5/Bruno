#pragma once

#include "TrioApiRequisites.h"

#include "ShaderStage.h"
#include "..\FX\GLSLGenerator.h"
#include "..\FX\HLSLGenerator.h"
#include <vector>

namespace BrunoEngine
{
	class BRUNO_API_EXPORT EffectCompiler
	{
	public:
#if BRUNO_DIRECTX
		static ID3DBlob* CompileShader(std::string path, const char* functionName, const char* profile, const D3D_SHADER_MACRO* defines, bool forceOptimization = false);

		static ID3D11VertexShader* CompileVSFromFile(ID3D11Device* device,
			std::string path,
			const char* functionName,
			const char* profile,
			const D3D_SHADER_MACRO* defines,
			ID3DBlob** byteCode,
			bool forceOptimization = false);

		static ID3D11PixelShader* CompilePSFromFile(ID3D11Device* device,
			std::string path,
			const char* functionName,
			const char* profile,
			const D3D_SHADER_MACRO* defines,
			bool forceOptimization = false);
#endif
		static void GenerateShaderFile(TrioFX::HLSLTree* tree, ShaderStage stage, std::string entryFunction, std::string &outputContent);
	};
}