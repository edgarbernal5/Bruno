#pragma once

#include <d3d12.h>
#include <wrl.h>

#include "CommandContext.h"

namespace Bruno
{
    class LinearAllocator;
    struct DynamicAllocation;
    
    class GraphicsContext : public CommandContext
    {
    public:
        // Constructor inyectando la command list que viene de tu CommandQueue
        GraphicsContext(GraphicsDevice& device, ID3D12GraphicsCommandList* commandList, ID3D12CommandAllocator* allocator, LinearAllocator* dynamicAllocator);

        DynamicAllocation AllocateDynamicSpace(size_t sizeInBytes);
        
        // --- BARRERAS Y ESTADOS ---
        void TransitionResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter);
    
        // --- CLEAR Y RENDER TARGETS ---
        void ClearRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtv, const float color[4]);
        void ClearDepth(D3D12_CPU_DESCRIPTOR_HANDLE dsv, float depth = 1.0f, uint8_t stencil = 0);
        void SetRenderTargets(uint32_t numRTVs, const D3D12_CPU_DESCRIPTOR_HANDLE* rtvs, D3D12_CPU_DESCRIPTOR_HANDLE* dsv = nullptr);

        // --- PIPELINE Y ESTADO GLOBAL ---
        void SetViewport(const D3D12_VIEWPORT& viewport);
        void SetScissorRect(const D3D12_RECT& rect);
        void SetPipelineState(ID3D12PipelineState* pso);
        void SetRootSignature(ID3D12RootSignature* rootSig);

        // --- ENLACE DE RECURSOS ---
        void SetDescriptorHeaps(ID3D12DescriptorHeap** ppHeaps, uint32_t count);
        void SetConstantBuffer(uint32_t rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS address);
        void SetDescriptorTable(uint32_t rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor);
        void SetDynamicDescriptorTable(uint32_t rootParameterIndex, D3D12_CPU_DESCRIPTOR_HANDLE cpuStagingDescriptor);
        
        // --- DIBUJO ---
        void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology);
        void SetVertexBuffer(const D3D12_VERTEX_BUFFER_VIEW&  view);
        void SetVertexBuffers(uint32_t startSlot, uint32_t count, const D3D12_VERTEX_BUFFER_VIEW* views);
        void SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* view);
        void DrawInstanced(uint32_t vertexCountPerInstance, uint32_t instanceCount, uint32_t startVertexLocation, uint32_t startInstanceLocation);
        void DrawIndexedInstanced(uint32_t indexCountPerInstance, uint32_t instanceCount, uint32_t startIndexLocation, int32_t baseVertexLocation, uint32_t startInstanceLocation);

        // Sube valores de 32 bits (floats, ints) directamente a la Root Signature
        void SetGraphicsRoot32BitConstants(uint32_t rootParameterIndex, uint32_t num32BitValuesToSet, const void* pSrcData, uint32_t destOffsetIn32BitValues = 0);
        
    private:
        LinearAllocator* m_dynamicAllocator = nullptr;
    };
}
