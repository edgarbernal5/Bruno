#include "stdafx.h"
#include "EffectCompiler.h"

namespace BrunoEngine
{
#if BRUNO_DIRECTX
	ID3DBlob* EffectCompiler::CompileShader(std::string path, const char* entryFunctionName, const char* profile, const D3D_SHADER_MACRO* defines, bool forceOptimization)
	{
		UINT flags = D3DCOMPILE_WARNINGS_ARE_ERRORS;
#ifdef _DEBUG
		flags |= D3DCOMPILE_DEBUG;
		if (forceOptimization == false)
			flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		ID3DBlob* compiledShader = nullptr;
		ID3DBlob* errorMessages = nullptr;

		std::wstring pathWS(path.begin(), path.end());
		//D3DX11CompileEffectFromFile
		//TO-DO: tomar en cuenta las clases y funciones de Microsoft
		//D3DX11CompileEffectFromFile

		HRESULT hr = D3DCompileFromFile(pathWS.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryFunctionName,
			profile, flags, 0, &compiledShader, &errorMessages);

		if (FAILED(hr))
		{
			if (errorMessages)
			{
				OutputDebugStringA((char*)errorMessages->GetBufferPointer());
				errorMessages->Release();
			}

			if (compiledShader)
			{
				compiledShader->Release();
			}
			return nullptr;
		}
		return compiledShader;
	}

	ID3D11VertexShader* EffectCompiler::CompileVSFromFile(ID3D11Device* device, std::string path, const char* functionName, const char* profile, const D3D_SHADER_MACRO* defines, ID3DBlob** byteCode, bool forceOptimization)
	{
		ID3DBlob* compiledShader = CompileShader(path, functionName, profile, defines, forceOptimization);
		ID3D11VertexShader* shader = nullptr;
		device->CreateVertexShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(),
			nullptr, &shader);

		if (byteCode != nullptr)
			*byteCode = compiledShader;

		return shader;
	}

	ID3D11PixelShader* EffectCompiler::CompilePSFromFile(ID3D11Device* device,
		std::string path,
		const char* functionName,
		const char* profile,
		const D3D_SHADER_MACRO* defines,
		bool forceOptimization)
	{
		ID3DBlob* compiledShader = CompileShader(path, functionName, profile, defines, forceOptimization);
		ID3D11PixelShader* shader = nullptr;
		device->CreatePixelShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(),
			nullptr, &shader);

		return shader;
	}
#endif
	void EffectCompiler::GenerateShaderFile(BrunoFX::HLSLTree* tree, ShaderStage stage, std::string entryFunction, std::string &outputContent)
	{
		//std::string outSourceCode;

#if BRUNO_DIRECTX
		BrunoFX::HLSLGenerator generator;
		switch (stage)
		{
		case ShaderStage::Vertex:
			generator.Generate(tree, BrunoFX::HLSLGenerator::Target_VertexShader, entryFunction.c_str(), false);
			break;
		case ShaderStage::Pixel:
			generator.Generate(tree, BrunoFX::HLSLGenerator::Target_PixelShader, entryFunction.c_str(), false);
			break;
		default:
			break;
		}
		//#elif OPENGL
#else
		GLSLGenerator generator;
		switch (stage)
		{
		case ShaderStage::Vertex:
			generator.Generate(tree, GLSLGenerator::Target_VertexShader, entryFunction.c_str());
			break;
		case ShaderStage::Pixel:
			generator.Generate(tree, GLSLGenerator::Target_FragmentShader, entryFunction.c_str());
			break;
		default:
			break;
		}
#endif
		outputContent = generator.GetResult();

		/*std::ofstream out(outputFilename);
		out << generator.GetResult();
		out.close();*/
	}
}