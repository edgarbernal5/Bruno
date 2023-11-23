#pragma once

#include "Context.h"
#include "Bruno/Math/Math.h"
#include <vector>

namespace Bruno
{
	class Resource;
	class Texture;
	class DepthBuffer;
	class GpuBuffer;
	class IndexBuffer;
	class VertexBuffer;
	class PipelineStateObject;
	class PipelineResourceSpace;
	struct PipelineInfo;

	class GraphicsContext : public Context
	{
	public:
		GraphicsContext(GraphicsDevice& device);
		~GraphicsContext();

		void ClearRenderTarget(const Texture& target, Math::Color color);
		void ClearDepthStencilTarget(const DepthBuffer& target, float depth, uint8_t stencil);
		
		void Draw(uint32_t vertexCount, uint32_t vertexStartOffset = 0);
		void DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation = 0, uint32_t baseVertexLocation = 0);
		void DrawInstanced(uint32_t vertexCountPerInstance, uint32_t instanceCount, uint32_t startVertexLocation = 0, uint32_t startInstanceLocation = 0);
		void DrawIndexedInstanced(uint32_t indexCountPerInstance, uint32_t instanceCount, uint32_t startIndexLocation, uint32_t baseVertexLocation, uint32_t startInstanceLocation);

		void SetBlendFactor(Math::Color blendFactor);
		void SetIndexBuffer(IndexBuffer& indexBuffer);
		void SetPipeline(const PipelineInfo& pipelineBinding);
		void SetPipelineResources(uint32_t spaceId, const PipelineResourceSpace& resources);
		void SetVertexBuffer(VertexBuffer& vertexBuffer);
		void SetViewport(const D3D12_VIEWPORT& viewPort);
		void SetScissorRect(const D3D12_RECT& rect);
		void SetStencilRef(uint32_t stencilRef);

		void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology);
		
	private:
		void SetTargets(uint32_t numRenderTargets, const D3D12_CPU_DESCRIPTOR_HANDLE renderTargets[], D3D12_CPU_DESCRIPTOR_HANDLE depthStencil);

		PipelineStateObject* m_currentPipeline = nullptr;
	};
}

