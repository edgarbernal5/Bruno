#include "brpch.h"
#include "GraphicsContext.h"

#include "RootSignature.h"
#include "PipelineStateObject.h"
#include "Texture.h"
#include "DepthBuffer.h"
#include "GPUBuffer.h"
#include "GraphicsDevice.h"

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

	void GraphicsContext::ClearDepthStencilTarget(const DepthBuffer& target, float depth, uint8_t stencil)
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

		if (mCurrentPipeline->mPipelineType == PipelineType::graphics)
		{
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
		BR_ASSERT(mCurrentPipeline);
		BR_ASSERT(resources.IsLocked());

		static const uint32_t maxNumHandlesPerBinding = 16;
		static const uint32_t singleDescriptorRangeCopyArray[maxNumHandlesPerBinding]{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,1 };

		const GPUBuffer* cbv = resources.GetCBV();
		const auto& uavs = resources.GetUAVs();
		const auto& srvs = resources.GetSRVs();
		const uint32_t numTableHandles = static_cast<uint32_t>(uavs.size() + srvs.size());
		D3D12_CPU_DESCRIPTOR_HANDLE handles[maxNumHandlesPerBinding]{};
		uint32_t currentHandleIndex = 0;
		BR_ASSERT(numTableHandles <= maxNumHandlesPerBinding);

		if (cbv)
		{
			auto& cbvMapping = mCurrentPipeline->mPipelineResourceMapping.mCbvMapping[spaceId];
			BR_ASSERT(cbvMapping.has_value());

			switch (mCurrentPipeline->mPipelineType)
			{
			case PipelineType::graphics:
				mCommandList->SetGraphicsRootConstantBufferView(cbvMapping.value(), cbv->mVirtualAddress);
				break;
			case PipelineType::compute:
				mCommandList->SetComputeRootConstantBufferView(cbvMapping.value(), cbv->mVirtualAddress);
				break;
			default:
				BR_ASSERT_ERROR("Invalid pipeline type");
				break;
			}
		}

		if (numTableHandles == 0)
		{
			return;
		}

		for (auto& uav : uavs)
		{
			if (uav.mResource->mType == GPUResourceType::Buffer)
			{
				handles[currentHandleIndex++] = static_cast<GPUBuffer*>(uav.mResource)->mUAVDescriptor.Cpu;
			}
			else if (uav.mResource->mType == GPUResourceType::Texture)
			{
				handles[currentHandleIndex++] = static_cast<Texture*>(uav.mResource)->mUAVDescriptor.Cpu;
			}
		}

		for (auto& srv : srvs)
		{
			if (srv.mResource->mType == GPUResourceType::Buffer)
			{
				handles[currentHandleIndex++] = static_cast<GPUBuffer*>(srv.mResource)->mSRVDescriptor.Cpu;
			}
			else if (srv.mResource->mType == GPUResourceType::Texture)
			{
				handles[currentHandleIndex++] = static_cast<Texture*>(srv.mResource)->mSRVDescriptor.Cpu;
			}
		}

		DescriptorHandle blockStart = mCurrentSRVHeap->Allocate(numTableHandles);
		mDevice.CopyDescriptors(1, &blockStart.Cpu, &numTableHandles, numTableHandles, handles, singleDescriptorRangeCopyArray, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		auto& tableMapping = mCurrentPipeline->mPipelineResourceMapping.mTableMapping[spaceId];
		BR_ASSERT(tableMapping.has_value());

		switch (mCurrentPipeline->mPipelineType)
		{
		case PipelineType::graphics:
			mCommandList->SetGraphicsRootDescriptorTable(tableMapping.value(), blockStart.Gpu);
			break;
		case PipelineType::compute:
			mCommandList->SetComputeRootDescriptorTable(tableMapping.value(), blockStart.Gpu);
			break;
		default:
			BR_ASSERT_ERROR("Invalid pipeline type");
			break;
		}
	}

	void GraphicsContext::SetViewport(const D3D12_VIEWPORT& viewport)
	{
		mCommandList->RSSetViewports(1, &viewport);
	}

	void GraphicsContext::SetScissorRect(const D3D12_RECT& rect)
	{
		mCommandList->RSSetScissorRects(1, &rect);
	}

	void GraphicsContext::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology)
	{

	}
}