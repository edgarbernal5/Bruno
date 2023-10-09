#pragma once

#include "Context.h"
#include "Bruno/Math/Math.h"
#include <vector>

namespace Bruno
{
	class Resource;
	class Texture;
	class GPUBuffer;
	class PipelineStateObject;
	class Texture;

	struct PipelineInfo
	{
		PipelineStateObject* mPipeline = nullptr;
		std::vector<Texture*> mRenderTargets;
		Texture* mDepthStencilTarget = nullptr;
	};

	struct PipelineResourceBinding
	{
		uint32_t mBindingIndex = 0;
		Resource* mResource = nullptr;
	};

	class PipelineResourceSpace
	{
	public:
		void SetCBV(GPUBuffer* resource);
		void SetSRV(const PipelineResourceBinding& binding);
		void SetUAV(const PipelineResourceBinding& binding);
		void Lock();

		const GPUBuffer* GetCBV() const { return mCBV; }
		const std::vector<PipelineResourceBinding>& GetUAVs() const { return mUAVs; }
		const std::vector<PipelineResourceBinding>& GetSRVs() const { return mSRVs; }

		bool IsLocked() const { return mIsLocked; }

	private:
		uint32_t GetIndexOfBindingIndex(const std::vector<PipelineResourceBinding>& bindings, uint32_t bindingIndex);

		//If a resource space needs more than one CBV, it is likely a design flaw, as you want to consolidate these as much
		//as possible if they have the same update frequency (which is contained by a PipelineResourceSpace). Of course,
		//you can freely change this to a vector like the others if you want.
		GPUBuffer* mCBV = nullptr;
		std::vector<PipelineResourceBinding> mUAVs;
		std::vector<PipelineResourceBinding> mSRVs;
		bool mIsLocked = false;

	};

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
		void SetPipeline(const PipelineInfo& pipelineBinding);
		void SetPipelineResources(uint32_t spaceId, const PipelineResourceSpace& resources);
		void SetViewport(const D3D12_VIEWPORT& viewPort);
		void SetScissorRect(const D3D12_RECT& rect);
		void SetStencilRef(uint32_t stencilRef);

		void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology);
		
	private:
		void SetTargets(uint32_t numRenderTargets, const D3D12_CPU_DESCRIPTOR_HANDLE renderTargets[], D3D12_CPU_DESCRIPTOR_HANDLE depthStencil);

		PipelineStateObject* mCurrentPipeline = nullptr;
	};
}

