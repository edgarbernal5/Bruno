#include "brpch.h"
#include "GraphicsContext.h"

#include "RootSignature.h"
#include "PipelineStateObject.h"
#include "Texture.h"

namespace Bruno
{
	GraphicsContext::GraphicsContext(GraphicsDevice& device) :
		Context(device, D3D12_COMMAND_LIST_TYPE_DIRECT)
	{
	}

	GraphicsContext::~GraphicsContext()
	{
	}

	void GraphicsContext::ClearRenderTarget(const Texture& target, Math::Color color)
	{
		mCommandList->ClearRenderTargetView(target.mRTVDescriptor.Cpu, color, 0, nullptr);
	}

	void GraphicsContext::ClearDepthStencilTarget(const Texture& target, float depth, uint8_t stencil)
	{
		mCommandList->ClearDepthStencilView(target.mDSVDescriptor.Cpu, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0, nullptr);
	}

	void GraphicsContext::Draw(uint32_t vertexCount, uint32_t vertexStartOffset)
	{
	}

	void GraphicsContext::DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation, uint32_t baseVertexLocation)
	{
	}

	void GraphicsContext::DrawInstanced(uint32_t vertexCountPerInstance, uint32_t instanceCount, uint32_t startVertexLocation, uint32_t startInstanceLocation)
	{
	}

	void GraphicsContext::DrawIndexedInstanced(uint32_t indexCountPerInstance, uint32_t instanceCount, uint32_t startIndexLocation, uint32_t baseVertexLocation, uint32_t startInstanceLocation)
	{
	}

	void GraphicsContext::SetBlendFactor(Math::Color blendFactor)
	{
	}

	void GraphicsContext::SetIndexBuffer(const GPUBuffer& indexBuffer)
	{
	}

	void GraphicsContext::SetTargets(uint32_t numRenderTargets, const D3D12_CPU_DESCRIPTOR_HANDLE renderTargets[], D3D12_CPU_DESCRIPTOR_HANDLE depthStencil)
	{
		mCommandList->OMSetRenderTargets(numRenderTargets, renderTargets, false, depthStencil.ptr != 0 ? &depthStencil : nullptr);
	}

	void GraphicsContext::SetPipeline(const PipelineInfo& pipelineBinding)
	{
		if (pipelineBinding.mPipeline->mPipelineType == PipelineType::compute)
		{
			mCommandList->SetPipelineState(pipelineBinding.mPipeline->GetD3D12PipelineState());
			mCommandList->SetComputeRootSignature(pipelineBinding.mPipeline->GetRootSignature()->GetD3D12RootSignature());
		}
		else
		{
			mCommandList->SetPipelineState(pipelineBinding.mPipeline->GetD3D12PipelineState());
			mCommandList->SetGraphicsRootSignature(pipelineBinding.mPipeline->GetRootSignature()->GetD3D12RootSignature());
		}

		mCurrentPipeline = pipelineBinding.mPipeline;

		if (mCurrentPipeline->mPipelineType == PipelineType::graphics) {
			D3D12_CPU_DESCRIPTOR_HANDLE renderTargetHandles[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
			D3D12_CPU_DESCRIPTOR_HANDLE depthStencilHandle{ 0 };

			size_t renderTargetCount = pipelineBinding.mRenderTargets.size();

			for (size_t targetIndex = 0; targetIndex < renderTargetCount; targetIndex++)
			{
				renderTargetHandles[targetIndex] = pipelineBinding.mRenderTargets[targetIndex]->mRTVDescriptor.Cpu;
			}

			if (pipelineBinding.mDepthStencilTarget)
			{
				depthStencilHandle = pipelineBinding.mDepthStencilTarget->mDSVDescriptor.Cpu;
			}

			SetTargets(static_cast<uint32_t>(renderTargetCount), renderTargetHandles, depthStencilHandle);
		}
	}

	void GraphicsContext::SetPipelineResources(uint32_t spaceId, const PipelineResourceSpace& resources)
	{

	}

	void PipelineResourceSpace::SetCBV(GPUBuffer* resource)
	{
	}
	void PipelineResourceSpace::SetSRV(const PipelineResourceBinding& binding)
	{
	}
	void PipelineResourceSpace::SetUAV(const PipelineResourceBinding& binding)
	{
	}
	void PipelineResourceSpace::Lock()
	{
	}
}