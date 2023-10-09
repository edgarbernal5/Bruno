#include "brpch.h"
#include "PipelineStateObject.h"

#include "GraphicsDevice.h"
#include "ShaderProgram.h"
#include "RootSignature.h"

namespace Bruno
{
	PipelineStateObject::PipelineStateObject(const D3D12_PIPELINE_STATE_STREAM_DESC& desc) :
		m_rootSignature{nullptr}
	{
		auto d3d12Device = Graphics::GetDevice()->GetD3DDevice();

		ThrowIfFailed(d3d12Device->CreatePipelineState(&desc, IID_PPV_ARGS(&m_d3d12PipelineState)));
	}

	PipelineStateObject::PipelineStateObject(const GraphicsPipelineDesc& desc, RootSignature* rootSignature) :
		m_rootSignature(rootSignature)
	{
		auto d3d12Device = Graphics::GetDevice()->GetD3DDevice();

		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
		pipelineDesc.NodeMask = 0;
		pipelineDesc.SampleMask = 0xFFFFFFFF;
		pipelineDesc.PrimitiveTopologyType = desc.mTopology;
		pipelineDesc.InputLayout = desc.mVertexShader->GetInputLayout();
		pipelineDesc.RasterizerState = desc.mRasterDesc;
		pipelineDesc.BlendState = desc.mBlendDesc;
		pipelineDesc.SampleDesc = desc.mSampleDesc;
		pipelineDesc.DepthStencilState = desc.mDepthStencilDesc;
		pipelineDesc.DSVFormat = desc.mRenderTargetDesc.mDepthStencilFormat;

		pipelineDesc.NumRenderTargets = desc.mRenderTargetDesc.mNumRenderTargets;
		for (uint32_t rtvIndex = 0; rtvIndex < pipelineDesc.NumRenderTargets; rtvIndex++)
		{
			pipelineDesc.RTVFormats[rtvIndex] = desc.mRenderTargetDesc.mRenderTargetFormats[rtvIndex];
		}

		if (desc.mVertexShader)
		{
			pipelineDesc.VS.pShaderBytecode = desc.mVertexShader->GetBlob()->GetBufferPointer();
			pipelineDesc.VS.BytecodeLength = desc.mVertexShader->GetBlob()->GetBufferSize();
		}

		if (desc.mPixelShader)
		{
			pipelineDesc.PS.pShaderBytecode = desc.mPixelShader->GetBlob()->GetBufferPointer();
			pipelineDesc.PS.BytecodeLength = desc.mPixelShader->GetBlob()->GetBufferSize();
		}
		pipelineDesc.pRootSignature = rootSignature->GetD3D12RootSignature();

		ThrowIfFailed(d3d12Device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&m_d3d12PipelineState)));
	}
}