#pragma once

#include "TrioAPI.h"


#include "..\..\TrioFX\GLSLGenerator.h"
#include "..\..\TrioFX\HLSLGenerator.h"
#include "StatesEnums.h"

using namespace CuadoFX;

namespace Cuado
{
	class TRIOAPI_DLL EffectCompiler
	{
	public:
#if TRIO_DIRECTX
		static ID3DBlob* CompileShader(string path, string functionName, string profile, const D3D_SHADER_MACRO* defines, bool forceOptimization = false);

		static ID3D11VertexShader* CompileVSFromFile(ID3D11Device* device,
			string path,
			string functionName,
			string profile,
			const D3D_SHADER_MACRO* defines,
			ID3DBlob** byteCode,
			bool forceOptimization = false);

		static ID3D11PixelShader* CompilePSFromFile(ID3D11Device* device,
			string path,
			string functionName,
			string profile,
			const D3D_SHADER_MACRO* defines,
			bool forceOptimization = false);
#endif
		static void GenerateShaderFile(HLSLTree* tree, ShaderStage stage, string entryFunction, string &outputContent);
	};
}