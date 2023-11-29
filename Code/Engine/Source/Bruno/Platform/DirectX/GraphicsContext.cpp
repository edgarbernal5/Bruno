#include "brpch.h"
#include "GraphicsContext.h"

#include "RootSignature.h"
#include "PipelineStateObject.h"
#include "Texture.h"
#include "DepthBuffer.h"
#include "GpuBuffer.h"
#include "GraphicsDevice.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"

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
		m_commandList->ClearRenderTargetView(target.m_rtvDescriptor.Cpu, color, 0, nullptr);
	}

	void GraphicsContext::ClearDepthStencilTarget(const DepthBuffer& target, float depth, uint8_t stencil)
	{
		m_commandList->ClearDepthStencilView(target.m_dsvDescriptor.Cpu, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0, nullptr);
	}

	void GraphicsContext::Draw(uint32_t vertexCount, uint32_t vertexStartOffset)
	{
		DrawInstanced(vertexCount, 1, vertexStartOffset, 0);
	}

	void GraphicsContext::DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation, uint32_t baseVertexLocation)
	{
		DrawIndexedInstanced(indexCount, 1, startIndexLocation, baseVertexLocation, 0);
	}

	void GraphicsContext::DrawInstanced(uint32_t vertexCountPerInstance, uint32_t instanceCount, uint32_t startVertexLocation, uint32_t startInstanceLocation)
	{
		m_commandList->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
	}

	void GraphicsContext::DrawIndexedInstanced(uint32_t indexCountPerInstance, uint32_t instanceCount, uint32_t startIndexLocation, uint32_t baseVertexLocation, uint32_t startInstanceLocation)
	{
		m_commandList->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
	}

	void GraphicsContext::SetBlendFactor(Math::Color blendFactor)
	{
		m_commandList->OMSetBlendFactor(blendFactor);
	}

	void GraphicsContext::SetIndexBuffer(IndexBuffer& indexBuffer)
	{
		m_commandList->IASetIndexBuffer(&indexBuffer.GetView());
	}

	void GraphicsContext::SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& indexBufferView)
	{
		m_commandList->IASetIndexBuffer(&indexBufferView);
	}

	void GraphicsContext::SetTargets(uint32_t numRenderTargets, const D3D12_CPU_DESCRIPTOR_HANDLE renderTargets[], D3D12_CPU_DESCRIPTOR_HANDLE depthStencil)
	{
		m_commandList->OMSetRenderTargets(numRenderTargets, renderTargets, false, depthStencil.ptr != 0 ? &depthStencil : nullptr);
	}

	void GraphicsContext::SetPipeline(const PipelineInfo& pipelineBinding, bool bindTargets)
	{
		if (pipelineBinding.Pipeline->m_pipelineType == PipelineType::Compute)
		{
			m_commandList->SetPipelineState(pipelineBinding.Pipeline->GetD3D12PipelineState());
			m_commandList->SetComputeRootSignature(pipelineBinding.Pipeline->GetRootSignature()->GetD3D12RootSignature());
		}
		else
		{
			m_commandList->SetPipelineState(pipelineBinding.Pipeline->GetD3D12PipelineState());
			m_commandList->SetGraphicsRootSignature(pipelineBinding.Pipeline->GetRootSignature()->GetD3D12RootSignature());
		}

		m_currentPipeline = pipelineBinding.Pipeline;

		if (bindTargets && m_currentPipeline->m_pipelineType == PipelineType::Graphics)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE renderTargetHandles[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
			D3D12_CPU_DESCRIPTOR_HANDLE depthStencilHandle{ 0 };

			size_t renderTargetCount = pipelineBinding.RenderTargets.size();

			for (size_t targetIndex = 0; targetIndex < renderTargetCount; targetIndex++)
			{
				renderTargetHandles[targetIndex] = pipelineBinding.RenderTargets[targetIndex]->m_rtvDescriptor.Cpu;
			}

			if (pipelineBinding.DepthStencilTarget)
			{
				depthStencilHandle = pipelineBinding.DepthStencilTarget->m_dsvDescriptor.Cpu;
			}

			SetTargets(static_cast<uint32_t>(renderTargetCount), renderTargetHandles, depthStencilHandle);
		}
	}

	void GraphicsContext::SetPipelineResources(uint32_t spaceId, const PipelineResourceSpace& resources)
	{
		BR_ASSERT(m_currentPipeline);
		BR_ASSERT(resources.IsLocked());

		static const uint32_t maxNumHandlesPerBinding = 16;
		static const uint32_t singleDescriptorRangeCopyArray[maxNumHandlesPerBinding]{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,1 };

		const GpuBuffer* cbv = resources.GetCBV();
		const auto& uavs = resources.GetUAVs();
		const auto& srvs = resources.GetSRVs();
		const uint32_t numTableHandles = static_cast<uint32_t>(uavs.size() + srvs.size());
		D3D12_CPU_DESCRIPTOR_HANDLE handles[maxNumHandlesPerBinding]{};
		uint32_t currentHandleIndex = 0;
		BR_ASSERT(numTableHandles <= maxNumHandlesPerBinding);

		if (cbv)
		{
			auto& cbvMapping = m_currentPipeline->m_pipelineResourceMapping.CbvMapping[spaceId];
			BR_ASSERT(cbvMapping.has_value());

			switch (m_currentPipeline->m_pipelineType)
			{
			case PipelineType::Graphics:
				m_commandList->SetGraphicsRootConstantBufferView(cbvMapping.value(), cbv->m_virtualAddress);
				break;
			case PipelineType::Compute:
				m_commandList->SetComputeRootConstantBufferView(cbvMapping.value(), cbv->m_virtualAddress);
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
			if (uav.Resource->m_resourceType == GPUResourceType::Buffer)
			{
				handles[currentHandleIndex++] = static_cast<GpuBuffer*>(uav.Resource)->m_uavDescriptor.Cpu;
			}
			else if (uav.Resource->m_resourceType == GPUResourceType::Texture)
			{
				handles[currentHandleIndex++] = static_cast<Texture*>(uav.Resource)->m_uavDescriptor.Cpu;
			}
		}

		for (auto& srv : srvs)
		{
			if (srv.Resource->m_resourceType == GPUResourceType::Buffer)
			{
				handles[currentHandleIndex++] = static_cast<GpuBuffer*>(srv.Resource)->m_srvDescriptor.Cpu;
			}
			else if (srv.Resource->m_resourceType == GPUResourceType::Texture)
			{
				handles[currentHandleIndex++] = static_cast<Texture*>(srv.Resource)->m_srvDescriptor.Cpu;
			}
		}

		DescriptorHandle blockStart = m_currentSrvHeap->Allocate(numTableHandles);
		m_device.CopyDescriptors(1, &blockStart.Cpu, &numTableHandles, numTableHandles, handles, singleDescriptorRangeCopyArray, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		auto& tableMapping = m_currentPipeline->m_pipelineResourceMapping.TableMapping[spaceId];
		BR_ASSERT(tableMapping.has_value());

		switch (m_currentPipeline->m_pipelineType)
		{
		case PipelineType::Graphics:
			m_commandList->SetGraphicsRootDescriptorTable(tableMapping.value(), blockStart.Gpu);
			break;
		case PipelineType::Compute:
			m_commandList->SetComputeRootDescriptorTable(tableMapping.value(), blockStart.Gpu);
			break;
		default:
			BR_ASSERT_ERROR("Invalid pipeline type");
			break;
		}
	}

	void GraphicsContext::SetVertexBuffer(VertexBuffer& vertexBuffer)
	{
		m_commandList->IASetVertexBuffers(0, 1, &vertexBuffer.GetView());
	}

	void GraphicsContext::SetVertexBuffer(const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView)
	{
		m_commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	}

	void GraphicsContext::SetViewport(const D3D12_VIEWPORT& viewport)
	{
		m_commandList->RSSetViewports(1, &viewport);
	}

	void GraphicsContext::SetScissorRect(const D3D12_RECT& rect)
	{
		m_commandList->RSSetScissorRects(1, &rect);
	}

	void GraphicsContext::SetStencilRef(uint32_t stencilRef)
	{
	}

	void GraphicsContext::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology)
	{
		m_commandList->IASetPrimitiveTopology(topology);
	}
}