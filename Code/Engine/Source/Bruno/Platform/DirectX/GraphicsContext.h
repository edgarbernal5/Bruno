#pragma once

#include <d3d12.h>
#include <wrl.h>

#include "Bruno/Math/Math.h"
#include "Bruno/Platform/DirectX/CommandContext.h"
#include "Bruno/Renderer/RHITypes.h"

namespace Bruno
{
    class ConstantBuffer;
    class IndexBuffer;
    class VertexBuffer;
    class GraphicsPipelineState;
    class DepthBuffer;
    class Texture2D;
    class RootSignature;
    class DynamicDescriptorAllocator;
    class LinearAllocator;
    struct DynamicAllocation;
    
    class GraphicsContext : public CommandContext
    {
    public:
        // Constructor inyectando la command list que viene de tu CommandQueue
        GraphicsContext(GraphicsDevice& device, ID3D12GraphicsCommandList* commandList, ID3D12CommandAllocator* allocator, LinearAllocator* dynamicAllocator);

        DynamicAllocation AllocateDynamicSpace(size_t sizeInBytes);
        
        // --- BARRERAS Y ESTADOS ---
        void TransitionResource(Texture2D* texture, ResourceState stateBefore, ResourceState stateAfter);
        void TransitionResource(DepthBuffer* depthBuffer, ResourceState stateBefore, ResourceState stateAfter);
        
        // --- CLEAR Y RENDER TARGETS ---
        void ClearRenderTarget(Texture2D* renderTarget, const Math::Color& color);
        void ClearDepth(DepthBuffer* depthBuffer, float depth = 1.0f, uint8_t stencil = 0);
        void SetRenderTargets(uint32_t numRTVs, Texture2D** renderTargets, DepthBuffer* depthBuffer = nullptr);
        
        // --- PIPELINE Y ESTADO GLOBAL ---
        void SetViewport(const Math::Viewport& viewport);
        void SetScissorRect(const Rect& rect);
        void SetPipelineState(GraphicsPipelineState* pso);
        void SetRootSignature(RootSignature* rootSig);

        // --- ENLACE DE RECURSOS ---
        void SetDescriptorHeaps(ID3D12DescriptorHeap** ppHeaps, uint32_t count);
        void SetConstantBuffer(uint32_t rootParameterIndex, ConstantBuffer* buffer);
        void SetConstantBuffer(uint32_t rootParameterIndex, const DynamicAllocation& allocation);
        void SetDescriptorTable(uint32_t rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor);
        void SetDynamicDescriptorTable(uint32_t rootParameterIndex, D3D12_CPU_DESCRIPTOR_HANDLE cpuStagingDescriptor);
        void SetTexture(uint32_t rootParameterIndex, Texture2D* texture);
        
        // --- DIBUJO ---
        void SetPrimitiveTopology(PrimitiveTopology topology);
        void SetVertexBuffer(uint32_t startSlot, VertexBuffer* vertexBuffer);
        void SetVertexBuffers(uint32_t startSlot, uint32_t count, VertexBuffer* views);
        void SetIndexBuffer(IndexBuffer* indexBuffer);
        
        void DrawInstanced(uint32_t vertexCountPerInstance, uint32_t instanceCount, uint32_t startVertexLocation, uint32_t startInstanceLocation);
        void DrawIndexedInstanced(uint32_t indexCountPerInstance, uint32_t instanceCount, uint32_t startIndexLocation, int32_t baseVertexLocation, uint32_t startInstanceLocation);

        // Sube valores de 32 bits (floats, ints) directamente a la Root Signature
        // Configura Push Constants (Vulkan) / 32-bit Root Constants (DX12)
        void SetPushConstants(uint32_t rootParameterIndex, uint32_t num32BitValues, const void* data, uint32_t destOffsetIn32BitValues = 0);
        
    private:
        inline D3D12_RESOURCE_STATES GetDX12ResourceState(ResourceState state);
        
        LinearAllocator* m_dynamicAllocator = nullptr;
        DynamicDescriptorAllocator* m_dynamicDescriptorAllocator = nullptr;
    };
}
