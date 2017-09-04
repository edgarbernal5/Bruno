#include "stdafx.h"
#include "EffectCompiler.h"


namespace Cuado
{

#if TRIO_DIRECTX
	ID3DBlob* EffectCompiler::CompileShader(string path, string functionName, string profile, const D3D_SHADER_MACRO* defines, bool forceOptimization)
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
		
		HRESULT hr = D3DCompileFromFile(pathWS.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, functionName.c_str(),
			profile.c_str(), flags, 0, &compiledShader, &errorMessages);

		if (FAILED(hr))
		{
			if (errorMessages)
			{
				OutputDebugStringA((char*)errorMessages->GetBufferPointer());
				MessageBoxA(0, (char*)errorMessages->GetBufferPointer(), 0, 0);
			}
			return nullptr;
		}
		return compiledShader;
	}


	ID3D11VertexShader* EffectCompiler::CompileVSFromFile(ID3D11Device* device, string path, string functionName, string profile, const D3D_SHADER_MACRO* defines, ID3DBlob** byteCode, bool forceOptimization)
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
		string path,
		string functionName,
		string profile,
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
	void EffectCompiler::GenerateShaderFile(HLSLTree* tree, ShaderStage stage, string entryFunction, string &outputContent)
	{
		//string outSourceCode;

#if TRIO_DIRECTX
		HLSLGenerator generator;
		switch (stage)
		{
		case ShaderStage::Vertex:
			generator.Generate(tree, HLSLGenerator::Target_VertexShader, entryFunction.c_str(), false);
			break;
		case ShaderStage::Pixel:
			generator.Generate(tree, HLSLGenerator::Target_PixelShader, entryFunction.c_str(), false);
			break;
		default:
			break;
		}
		//#elif TRIO_OPENGL
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