#include "brpch.h"
#include "GraphicsContext.h"

#include "DynamicAllocation.h"

namespace Bruno
{
    GraphicsContext::GraphicsContext(GraphicsDevice& device, ID3D12GraphicsCommandList* commandList, ID3D12CommandAllocator* allocator, LinearAllocator* dynamicAllocator) : 
        CommandContext(device, D3D12_COMMAND_LIST_TYPE_DIRECT, commandList, allocator), m_dynamicAllocator(dynamicAllocator)
    {
    }
    
    DynamicAllocation GraphicsContext::AllocateDynamicSpace(size_t sizeInBytes)
    {
        // Delegamos la petición al allocator del frame actual
        // Esto es O(1), solo avanza un puntero interno.
        return m_dynamicAllocator->Allocate(sizeInBytes); 
    }
    
    void GraphicsContext::TransitionResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter)
    {
        if (stateBefore == stateAfter)
        {
            return; // Optimización simple
        }
        
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
        m_commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0, nullptr);
    }

    void GraphicsContext::SetRenderTargets(uint32_t numRTVs, const D3D12_CPU_DESCRIPTOR_HANDLE* rtvs, D3D12_CPU_DESCRIPTOR_HANDLE* dsv)
    {
        // FALSE indica que los RTVs no son necesariamente contiguos en memoria
        m_commandList->OMSetRenderTargets(numRTVs, rtvs, FALSE, dsv);
    }

    void GraphicsContext::SetViewport(const D3D12_VIEWPORT& viewport)
    {
        m_commandList->RSSetViewports(1, &viewport);
    }

    void GraphicsContext::SetScissorRect(const D3D12_RECT& rect)
    {
        m_commandList->RSSetScissorRects(1, &rect);
    }

    void GraphicsContext::SetPipelineState(ID3D12PipelineState* pso)
    {
        m_commandList->SetPipelineState(pso);
    }

    void GraphicsContext::SetRootSignature(ID3D12RootSignature* rootSig)
    {
        m_commandList->SetGraphicsRootSignature(rootSig);
    }

    void GraphicsContext::SetDescriptorHeaps(ID3D12DescriptorHeap** ppHeaps, uint32_t count)
    {
        m_commandList->SetDescriptorHeaps(count, ppHeaps);
    }

    void GraphicsContext::SetConstantBuffer(uint32_t rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS address)
    {
        m_commandList->SetGraphicsRootConstantBufferView(rootParameterIndex, address);
    }

    void GraphicsContext::SetDescriptorTable(uint32_t rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor)
    {
        m_commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, baseDescriptor);
    }

    void GraphicsContext::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology)
    {
        m_commandList->IASetPrimitiveTopology(topology);
    }

    void GraphicsContext::SetVertexBuffer(const D3D12_VERTEX_BUFFER_VIEW& view)
    {
        m_commandList->IASetVertexBuffers(0, 1, &view);
    }

    void GraphicsContext::SetVertexBuffers(uint32_t startSlot, uint32_t count, const D3D12_VERTEX_BUFFER_VIEW* views)
    {
        m_commandList->IASetVertexBuffers(startSlot, count, views);
    }

    void GraphicsContext::SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* view)
    {
        m_commandList->IASetIndexBuffer(view);
    }

    void GraphicsContext::DrawInstanced(uint32_t vertexCountPerInstance, uint32_t instanceCount, uint32_t startVertexLocation, uint32_t startInstanceLocation)
    {
        m_commandList->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
    }

    void GraphicsContext::DrawIndexedInstanced(uint32_t indexCountPerInstance, uint32_t instanceCount, uint32_t startIndexLocation, int32_t baseVertexLocation, uint32_t startInstanceLocation)
    {
        m_commandList->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
    }

    void GraphicsContext::SetGraphicsRoot32BitConstants(uint32_t rootParameterIndex, uint32_t num32BitValuesToSet, const void* pSrcData, uint32_t destOffsetIn32BitValues)
    {
        m_commandList->SetGraphicsRoot32BitConstants(
            rootParameterIndex, 
            num32BitValuesToSet, 
            pSrcData, 
            destOffsetIn32BitValues
        );
    }
}
