#include "brpch.h"
#include "PipelineStateObjectCache.h"
#include <Bruno/Platform/DirectX/RootSignature.h>
#include <Bruno/Platform/DirectX/Shader.h>
#include <Bruno/Platform/DirectX/PipelineStateObject.h>

namespace Bruno
{
	void PipelineStateObjectCache::Initialize(DXGI_FORMAT backbufferFormat, DXGI_FORMAT depthStencilFormat)
	{
		auto shader = GetShader(RenderPsoId::UnlitColored);
		
		GraphicsPipelineDesc meshPipelineDesc = GetDefaultGraphicsPipelineDesc();
		meshPipelineDesc.VertexShader = shader->GetShaderProgram(Shader::ShaderProgramType::Vertex);
		meshPipelineDesc.PixelShader = shader->GetShaderProgram(Shader::ShaderProgramType::Pixel);
		meshPipelineDesc.RenderTargetDesc.DepthStencilFormat = depthStencilFormat;
		meshPipelineDesc.RenderTargetDesc.RenderTargetsCount = 1;
		meshPipelineDesc.RenderTargetDesc.RenderTargetFormats[0] = backbufferFormat;
		meshPipelineDesc.DepthStencilDesc.DepthEnable = false;
		meshPipelineDesc.DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

		PipelineResourceMapping resourceMapping;
		auto rootsignature = shader->CreateRootSignature(resourceMapping);
		m_cache[RenderPsoId::UnlitColored] = std::make_shared<PipelineStateObject>(meshPipelineDesc, rootsignature, resourceMapping);
	}

	std::shared_ptr<PipelineStateObject> PipelineStateObjectCache::Get(RenderPsoId psoId)
	{
		return m_cache[psoId];
	}

	std::shared_ptr<Shader> PipelineStateObjectCache::GetShader(RenderPsoId psoId)
	{
		if (psoId == RenderPsoId::UnlitColored)
			return Graphics::GetShaderCache().Get(L"Shaders/UnlitColor.hlsl");
		return Graphics::GetShaderCache().Get(L"");
	}

}