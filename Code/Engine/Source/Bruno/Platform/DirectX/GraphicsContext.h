#pragma once

#include "Context.h"
#include <Bruno/Math/Math.h>

namespace Bruno
{
	class Texture;
	class GPUBuffer;

	class GraphicsContext : public Context
	{
	public:
		GraphicsContext(GraphicsDevice& device);
		~GraphicsContext();

		void ClearRenderTarget(const Texture& target, Math::Color color);
		void ClearDepthStencilTarget(const Texture& target, float depth, uint8_t stencil);

		
		void Draw(uint32_t vertexCount, uint32_t vertexStartOffset = 0);
		void DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation = 0, uint32_t baseVertexLocation = 0);
		void DrawInstanced(uint32_t vertexCountPerInstance, uint32_t instanceCount, uint32_t startVertexLocation = 0, uint32_t startInstanceLocation = 0);
		void DrawIndexedInstanced(uint32_t indexCountPerInstance, uint32_t instanceCount, uint32_t startIndexLocation, uint32_t baseVertexLocation, uint32_t startInstanceLocation);

		void SetBlendFactor(Math::Color blendFactor);
		void SetIndexBuffer(const GPUBuffer& indexBuffer);
		void SetViewport(const D3D12_VIEWPORT& viewPort);
		void SetScissorRect(const D3D12_RECT& rect);
		void SetStencilRef(uint32_t stencilRef);

		void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology);


	private:

	};
}

