#include "brpch.h"
#include "PipelineStateObject.h"

#include "GraphicsDevice.h"
#include "ShaderProgram.h"
#include "RootSignature.h"

namespace Bruno
{
	bool SortPipelineBindings(PipelineResourceBinding a, PipelineResourceBinding b)
	{
		return a.BindingIndex < b.BindingIndex;
	}

	PipelineStateObject::PipelineStateObject(const D3D12_PIPELINE_STATE_STREAM_DESC& desc) :
		m_rootSignature{nullptr}
	{
		auto d3d12Device = Graphics::GetDevice()->GetD3DDevice();

		ThrowIfFailed(d3d12Device->CreatePipelineState(&desc, IID_PPV_ARGS(&m_d3d12PipelineState)));
	}

	PipelineStateObject::PipelineStateObject(const GraphicsPipelineDesc& desc, RootSignature* rootSignature, PipelineResourceMapping& pipelineResourceMapping) :
		m_rootSignature(rootSignature)
	{
		auto d3d12Device = Graphics::GetDevice()->GetD3DDevice();

		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
		pipelineDesc.NodeMask = 0;
		pipelineDesc.SampleMask = 0xFFFFFFFF;
		pipelineDesc.PrimitiveTopologyType = desc.Topology;
		pipelineDesc.InputLayout = desc.VertexShader->GetInputLayout();
		pipelineDesc.RasterizerState = desc.RasterDesc;
		pipelineDesc.BlendState = desc.BlendDesc;
		pipelineDesc.SampleDesc = desc.SampleDesc;
		pipelineDesc.DepthStencilState = desc.DepthStencilDesc;
		pipelineDesc.DSVFormat = desc.RenderTargetDesc.DepthStencilFormat;

		pipelineDesc.NumRenderTargets = desc.RenderTargetDesc.RenderTargetsCount;
		for (uint32_t rtvIndex = 0; rtvIndex < pipelineDesc.NumRenderTargets; rtvIndex++)
		{
			pipelineDesc.RTVFormats[rtvIndex] = desc.RenderTargetDesc.RenderTargetFormats[rtvIndex];
		}

		if (desc.VertexShader)
		{
			pipelineDesc.VS.pShaderBytecode = desc.VertexShader->GetBlob()->GetBufferPointer();
			pipelineDesc.VS.BytecodeLength = desc.VertexShader->GetBlob()->GetBufferSize();
		}

		if (desc.PixelShader)
		{
			pipelineDesc.PS.pShaderBytecode = desc.PixelShader->GetBlob()->GetBufferPointer();
			pipelineDesc.PS.BytecodeLength = desc.PixelShader->GetBlob()->GetBufferSize();
		}
		pipelineDesc.pRootSignature = rootSignature->GetD3D12RootSignature();
		
		ThrowIfFailed(d3d12Device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&m_d3d12PipelineState)));
		m_pipelineResourceMapping = pipelineResourceMapping;
	}

	void PipelineResourceSpace::SetCBV(GPUBuffer* resource)
	{
		if (m_isLocked)
		{
			if (m_cbv == nullptr)
			{
				BR_ASSERT_ERROR("Setting unused binding in a locked resource space");
			}
			else
			{
				m_cbv = resource;
			}
		}
		else
		{
			m_cbv = resource;
		}
	}

	void PipelineResourceSpace::SetSRV(const PipelineResourceBinding& binding)
	{
		uint32_t currentIndex = GetIndexOfBindingIndex(m_srvs, binding.BindingIndex);

		if (m_isLocked)
		{
			if (currentIndex == UINT_MAX)
			{
				BR_ASSERT_ERROR("Setting unused binding in a locked resource space");
			}
			else
			{
				m_srvs[currentIndex] = binding;
			}
		}
		else
		{
			if (currentIndex == UINT_MAX)
			{
				m_srvs.push_back(binding);
				std::sort(m_srvs.begin(), m_srvs.end(), SortPipelineBindings);
			}
			else
			{
				m_srvs[currentIndex] = binding;
			}
		}
	}

	void PipelineResourceSpace::SetUAV(const PipelineResourceBinding& binding)
	{
		uint32_t currentIndex = GetIndexOfBindingIndex(m_uavs, binding.BindingIndex);

		if (m_isLocked)
		{
			if (currentIndex == UINT_MAX)
			{
				BR_ASSERT_ERROR("Setting unused binding in a locked resource space");
			}
			else
			{
				m_uavs[currentIndex] = binding;
			}
		}
		else
		{
			if (currentIndex == UINT_MAX)
			{
				m_uavs.push_back(binding);
				std::sort(m_uavs.begin(), m_uavs.end(), SortPipelineBindings);
			}
			else
			{
				m_uavs[currentIndex] = binding;
			}
		}
	}

	void PipelineResourceSpace::Lock()
	{
		m_isLocked = true;
	}

	uint32_t PipelineResourceSpace::GetIndexOfBindingIndex(const std::vector<PipelineResourceBinding>& bindings, uint32_t bindingIndex)
	{
		const uint32_t numBindings = static_cast<uint32_t>(bindings.size());
		for (uint32_t vectorIndex = 0; vectorIndex < numBindings; vectorIndex++)
		{
			if (bindings.at(vectorIndex).BindingIndex == bindingIndex)
			{
				return vectorIndex;
			}
		}

		return UINT_MAX;
	}
}