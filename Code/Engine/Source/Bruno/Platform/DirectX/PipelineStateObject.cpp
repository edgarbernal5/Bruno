#include "brpch.h"
#include "PipelineStateObject.h"

#include "GraphicsDevice.h"
#include "ShaderProgram.h"
#include "RootSignature.h"

namespace Bruno
{
	bool SortPipelineBindings(PipelineResourceBinding a, PipelineResourceBinding b)
	{
		return a.mBindingIndex < b.mBindingIndex;
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
		mPipelineResourceMapping = pipelineResourceMapping;
	}

	void PipelineResourceSpace::SetCBV(GPUBuffer* resource)
	{
		if (mIsLocked)
		{
			if (mCBV == nullptr)
			{
				BR_ASSERT_ERROR("Setting unused binding in a locked resource space");
			}
			else
			{
				mCBV = resource;
			}
		}
		else
		{
			mCBV = resource;
		}
	}

	void PipelineResourceSpace::SetSRV(const PipelineResourceBinding& binding)
	{
		uint32_t currentIndex = GetIndexOfBindingIndex(mSRVs, binding.mBindingIndex);

		if (mIsLocked)
		{
			if (currentIndex == UINT_MAX)
			{
				BR_ASSERT_ERROR("Setting unused binding in a locked resource space");
			}
			else
			{
				mSRVs[currentIndex] = binding;
			}
		}
		else
		{
			if (currentIndex == UINT_MAX)
			{
				mSRVs.push_back(binding);
				std::sort(mSRVs.begin(), mSRVs.end(), SortPipelineBindings);
			}
			else
			{
				mSRVs[currentIndex] = binding;
			}
		}
	}

	void PipelineResourceSpace::SetUAV(const PipelineResourceBinding& binding)
	{
		uint32_t currentIndex = GetIndexOfBindingIndex(mUAVs, binding.mBindingIndex);

		if (mIsLocked)
		{
			if (currentIndex == UINT_MAX)
			{
				BR_ASSERT_ERROR("Setting unused binding in a locked resource space");
			}
			else
			{
				mUAVs[currentIndex] = binding;
			}
		}
		else
		{
			if (currentIndex == UINT_MAX)
			{
				mUAVs.push_back(binding);
				std::sort(mUAVs.begin(), mUAVs.end(), SortPipelineBindings);
			}
			else
			{
				mUAVs[currentIndex] = binding;
			}
		}
	}

	void PipelineResourceSpace::Lock()
	{
		mIsLocked = true;
	}

	uint32_t PipelineResourceSpace::GetIndexOfBindingIndex(const std::vector<PipelineResourceBinding>& bindings, uint32_t bindingIndex)
	{
		const uint32_t numBindings = static_cast<uint32_t>(bindings.size());
		for (uint32_t vectorIndex = 0; vectorIndex < numBindings; vectorIndex++)
		{
			if (bindings.at(vectorIndex).mBindingIndex == bindingIndex)
			{
				return vectorIndex;
			}
		}

		return UINT_MAX;
	}
}