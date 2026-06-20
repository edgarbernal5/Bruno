#include "brpch.h"
#include "GraphicsContext_Gem.h"

namespace Bruno::DX
{
    GraphicsContext::GraphicsContext(DX::GraphicsDevice& device) : 
        CommandContext(device, D3D12_COMMAND_LIST_TYPE_DIRECT)
    {
    }

    void GraphicsContext::TransitionResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES stateBefore,
        D3D12_RESOURCE_STATES stateAfter)
    {
        if (stateBefore == stateAfter) return; // Optimización simple

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = resource;
        barrier.Transition.StateBefore = stateBefore;
        barrier.Transition.StateAfter = stateAfter;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

        m_commandList->ResourceBarrier(1, &barrier);
    }

    void GraphicsContext::ClearRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtv, const float color[4])
    {
        m_commandList->ClearRenderTargetView(rtv, color, 0, nullptr);
    }

    void GraphicsContext::ClearDepth(D3D12_CPU_DESCRIPTOR_HANDLE dsv, float depth, uint8_t stencil)
    {
    }

    void GraphicsContext::SetRenderTargets(uint32_t count, const D3D12_CPU_DESCRIPTOR_HANDLE* rtvs,
        D3D12_CPU_DESCRIPTOR_HANDLE* dsv)
    {
    }

    void GraphicsContext::SetViewport(const D3D12_VIEWPORT& viewport)
    {
    }

    void GraphicsContext::SetScissorRect(const D3D12_RECT& rect)
    {
    }

    void GraphicsContext::SetPipelineState(ID3D12PipelineState* pso)
    {
    }

    void GraphicsContext::SetRootSignature(ID3D12RootSignature* rootSig)
    {
    }

    void GraphicsContext::SetDescriptorHeaps(ID3D12DescriptorHeap** heaps, uint32_t count)
    {
    }

    void GraphicsContext::SetConstantBuffer(uint32_t rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS address)
    {
    }

    void GraphicsContext::SetDescriptorTable(uint32_t rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor)
    {
    }

    void GraphicsContext::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology)
    {
    }

    void GraphicsContext::SetVertexBuffers(uint32_t startSlot, uint32_t count, const D3D12_VERTEX_BUFFER_VIEW* views)
    {
    }

    void GraphicsContext::SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* view)
    {
    }

    void GraphicsContext::DrawIndexedInstanced(uint32_t indexCountPerInstance, uint32_t instanceCount,
        uint32_t startIndexLocation, int32_t baseVertexLocation, uint32_t startInstanceLocation)
    {
    }
}
