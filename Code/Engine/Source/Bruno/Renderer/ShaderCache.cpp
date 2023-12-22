#include "brpch.h"
#include "ShaderCache.h"

#include "Bruno/Platform/DirectX/Shader.h"
#include "Bruno/Platform/DirectX/RootSignature.h"
#include "Bruno/Platform/DirectX/PipelineStateObject.h"
#include <filesystem>

namespace Bruno
{
	void ShaderCache::Initialize()
	{
		for (size_t id = 0; id < (size_t)RenderShaderId::Count; ++id)
		{
			std::string shaderFileName = GetShaderNameById((RenderShaderId)id);
			std::filesystem::path shaderPath("Shaders");
			shaderPath /= shaderFileName;

			auto shader = Load(shaderPath);
		}
	}

	std::shared_ptr<Shader> ShaderCache::Get(const std::wstring& filename)
	{
		auto it = m_cache.find(filename);
		if (it == m_cache.end())
		{
			return Load(filename);
		}
		return it->second;
	}

	std::string ShaderCache::GetShaderNameById(RenderShaderId shaderId)
	{
		switch (shaderId)
		{
		case Bruno::RenderShaderId::UnlitColored:
			return "UnlitColor.hlsl";
		case Bruno::RenderShaderId::OpaqueForward:
			return "Opaque.hlsl";
		default:
			break;
		}
		return "";
	}

	std::shared_ptr<Shader> ShaderCache::Load(const std::wstring& filename)
	{
		return std::make_shared<Shader>(filename);
	}

	void  PipelineStateObjectCache::Initialize()
	{
		//auto unlitColoredShader = 
		/*PipelineResourceMapping resourceMapping;
		auto rootSignature = std::make_shared<RootSignature>(meshResourceLayout, resourceMapping);

		GraphicsPipelineDesc meshPipelineDesc = GetDefaultGraphicsPipelineDesc();
		meshPipelineDesc.VertexShader = renderObjectDesc->Shader->GetShaderProgram(Shader::ShaderProgramType::Vertex);
		meshPipelineDesc.PixelShader = renderObjectDesc->Shader->GetShaderProgram(Shader::ShaderProgramType::Pixel);
		meshPipelineDesc.RenderTargetDesc.DepthStencilFormat = surface->GetDepthBufferFormat();
		meshPipelineDesc.RenderTargetDesc.RenderTargetsCount = 1;
		meshPipelineDesc.RenderTargetDesc.RenderTargetFormats[0] = surface->GetSurfaceFormat();
		meshPipelineDesc.DepthStencilDesc.DepthEnable = false;
		meshPipelineDesc.DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

		renderObjectDesc->PipelineObject = std::make_shared<PipelineStateObject>(meshPipelineDesc, rootSignature, resourceMapping); */
	}
}