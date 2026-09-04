#include "brpch.h"
#include "GraphicsContext.h"

#include <entt/entt.hpp>

#include "ConstantBuffer.h"
#include "ConstantBufferBase.h"
#include "D3DFunctions.h"
#include "DepthBuffer.h"
#include "DynamicAllocation.h"
#include "DynamicDescriptorAllocator.h"
#include "GraphicsPipelineState.h"
#include "IndexBuffer.h"
#include "RootSignature.h"
#include "Texture2D.h"
#include "VertexBuffer.h"

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

    void GraphicsContext::TransitionResource(GraphicsResource* resource, ResourceState newState)
    {
        if (!resource) return;

        ResourceState stateBefore = resource->GetCurrentState();
        
        // Evitamos emitir barreras nulas en la GPU si el recurso ya está en el estado correcto
        if (stateBefore == newState)
        {
            return;
        }
        
        D3D12_RESOURCE_STATES dxStateBefore = D3DFunctions::GetDX12ResourceState(stateBefore);
        D3D12_RESOURCE_STATES dxNewState = D3DFunctions::GetDX12ResourceState(newState);

        ID3D12Resource* nativeResource = resource->GetNativeResource();

        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            nativeResource,
            dxStateBefore,
            dxNewState
        );

        m_commandList->ResourceBarrier(1, &barrier);
        
        resource->SetCurrentState(newState);
    }

    void GraphicsContext::ClearRenderTarget(Texture2D* renderTarget, const Math::Color& color)
    {
        // Extraemos el Descriptor Handle (D3D12_CPU_DESCRIPTOR_HANDLE)
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderTarget->GetRTV();

        m_commandList->ClearRenderTargetView(rtvHandle, color, 0, nullptr);
    }

    void GraphicsContext::ClearDepth(const DepthBuffer* depthBuffer, float depth, uint8_t stencil)
    {
        // Extraemos el Descriptor Handle (D3D12_CPU_DESCRIPTOR_HANDLE)
        D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthBuffer->GetView();

        m_commandList->ClearDepthStencilView(
            dsvHandle,
            D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
            depth,
            stencil,
            0,
            nullptr
        );
    }

    void GraphicsContext::SetRenderTargets(uint32_t numRTVs, Texture2D** renderTargets, DepthBuffer* depthBuffer)
    {
        // DirectX 12 permite un máximo de 8 Render Targets simultáneos por lo general.
        BR_ASSERT(numRTVs <= 8, "Se superó el límite máximo de Render Targets.");

        // Creamos un arreglo temporal en el Stack (¡súper rápido, 0 asignaciones en el Heap!)
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[8];
    
        for (uint32_t i = 0; i < numRTVs; ++i)
        {
            // Extraemos el Handle nativo de cada textura agnóstica
            rtvHandles[i] = renderTargets[i]->GetRTV();
        }

        // Preparamos el Depth Stencil (si existe)
        D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
        D3D12_CPU_DESCRIPTOR_HANDLE* pDsvHandle = nullptr;

        if (depthBuffer)
        {
            dsvHandle = depthBuffer->GetView();
            pDsvHandle = &dsvHandle;
        }

        // Llamada nativa
        // Parámetros: Num RTVs, Puntero al array, RTs Contiguos (FALSE porque los sacamos manualmente), Puntero al DSV
        m_commandList->OMSetRenderTargets(numRTVs, rtvHandles, FALSE, pDsvHandle);
    }

    void GraphicsContext::SetViewport(const Math::Viewport& viewport)
    {
        m_commandList->RSSetViewports(1, viewport.Get12());
    }

    void GraphicsContext::SetScissorRect(const Rect& rect)
    {
        m_commandList->RSSetScissorRects(1, rect.Get12());
    }

    void GraphicsContext::SetPipelineState(GraphicsPipelineState* pso)
    {
        m_commandList->SetPipelineState(pso->GetNative());
    }

    void GraphicsContext::SetRootSignature(RootSignature* rootSig)
    {
        m_commandList->SetGraphicsRootSignature(rootSig->GetNative());
    }

    void GraphicsContext::SetDescriptorHeaps(std::initializer_list<DescriptorAllocator*> heaps)
    {
        // Validación de seguridad nivel AAA: El hardware no soporta más de 2 heaps.
        if (heaps.size() > 2)
        {
            throw std::runtime_error("DirectX 12 solo soporta un maximo de 2 Descriptor Heaps bindeados simultaneamente.");
        }
        
        std::array<ID3D12DescriptorHeap*, 2> nativeHeaps = { nullptr, nullptr };
    
        uint32_t i = 0;
        for (auto* heap : heaps)
        {
            nativeHeaps[i] = heap->GetHeap();
            i++;
        }

        m_commandList->SetDescriptorHeaps(static_cast<UINT>(heaps.size()), nativeHeaps.data());
    }

    void GraphicsContext::SetConstantBuffer(uint32_t rootParameterIndex, const ConstantBufferBase& cb)
    {
        m_commandList->SetGraphicsRootConstantBufferView(rootParameterIndex, cb.GetGPUAddress());
    }

    void GraphicsContext::SetConstantBuffer(uint32_t rootParameterIndex, const DynamicAllocation& allocation)
    {
        m_commandList->SetGraphicsRootConstantBufferView(rootParameterIndex, allocation.GPUAddress);
    }

    void GraphicsContext::SetDescriptorTable(uint32_t rootParameterIndex, const DescriptorAllocator& descriptorAllocator)
    {
        // En tu arquitectura, el DescriptorAllocator encapsula el heap nativo.
        // Extraemos el handle de memoria inicial de la GPU (Offset 0)
        D3D12_GPU_DESCRIPTOR_HANDLE baseHandle = descriptorAllocator.GetHeap()->GetGPUDescriptorHandleForHeapStart();
        
        // Lo bindeamos a la Root Signature
        m_commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, baseHandle);
    }

    void GraphicsContext::SetDynamicDescriptorTable(uint32_t rootParameterIndex, D3D12_CPU_DESCRIPTOR_HANDLE cpuStagingDescriptor)
    {
        // 1. Asegurarnos de que el Heap Dinámico está seteado en el Command List
        ID3D12DescriptorHeap* heaps[] = { m_dynamicDescriptorAllocator->GetHeap() };
        m_commandList->SetDescriptorHeaps(1, heaps);

        // 2. Copiar del Almacén al Mostrador
        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = m_dynamicDescriptorAllocator->CopyDescriptor(m_device, cpuStagingDescriptor);

        // 3. Bindear en la Root Signature
        m_commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, gpuHandle);
    }

    void GraphicsContext::SetTexture(uint32_t rootParameterIndex, Texture2D* texture)
    {
        // 1. Extraemos el Descriptor Handle nativo (CPU) de la textura agnóstica.
        D3D12_CPU_DESCRIPTOR_HANDLE srv = texture->GetSRV();

        // 2. Usamos tu método existente que copia este descriptor a un Heap dinámico 
        //    visible por la GPU y enlaza el Descriptor Table.
        SetDynamicDescriptorTable(rootParameterIndex, srv);
    }

    void GraphicsContext::SetPrimitiveTopology(PrimitiveTopology topology)
    {
        D3D_PRIMITIVE_TOPOLOGY dxTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    
        // Traducción estática ultra rápida
        switch (topology) {
        case PrimitiveTopology::PointList:     dxTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST; break;
        case PrimitiveTopology::LineList:      dxTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST; break;
        case PrimitiveTopology::LineStrip:     dxTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP; break;
        case PrimitiveTopology::TriangleList:  dxTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST; break;
        case PrimitiveTopology::TriangleStrip: dxTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP; break;
        }
    
        m_commandList->IASetPrimitiveTopology(dxTopology);
    }

    void GraphicsContext::SetVertexBuffer(uint32_t startSlot, VertexBuffer* vertexBuffer)
    {
        D3D12_VERTEX_BUFFER_VIEW view = vertexBuffer->GetView();
        m_commandList->IASetVertexBuffers(startSlot, 1, &view);
    }

    void GraphicsContext::SetVertexBuffers(uint32_t startSlot, std::initializer_list<const VertexBuffer*> buffers)
    {
        // DX12 soporta múltiples slots, usamos un array fijo rápido
        std::array<D3D12_VERTEX_BUFFER_VIEW, 8> views = {};
        uint32_t i = 0;
    
        for (const auto* vb : buffers)
        {
            views[i++] = vb->GetView();
        }
    
        m_commandList->IASetVertexBuffers(startSlot, static_cast<UINT>(buffers.size()), views.data());
    }

    void GraphicsContext::SetIndexBuffer(IndexBuffer* indexBuffer)
    {
        auto view = indexBuffer->GetView();
        m_commandList->IASetIndexBuffer(&view);
    }

    void GraphicsContext::CopyBuffer(GraphicsResource* dest, GraphicsResource* src, size_t size)
    {
        // 1. Validación estricta AAA para evitar pantallazos azules (TDRs)
        if (!dest || !src)
        {
            throw std::invalid_argument("Punteros nulos pasados a CopyBuffer.");
        }

        if (size == 0) 
        {
            return; 
        }
        
        // Encolar la transferencia masiva de datos en la CommandList del contexto
        // Parámetros: Destino, Offset Destino, Origen, Offset Origen, Tamaño
        m_commandList->CopyBufferRegion(dest->GetNativeResource(), 0, src->GetNativeResource(), 0, size);
    }

    void GraphicsContext::DrawInstanced(uint32_t vertexCountPerInstance, uint32_t instanceCount, uint32_t startVertexLocation, uint32_t startInstanceLocation)
    {
        m_commandList->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
    }

    void GraphicsContext::DrawIndexedInstanced(uint32_t indexCountPerInstance, uint32_t instanceCount, uint32_t startIndexLocation, int32_t baseVertexLocation, uint32_t startInstanceLocation)
    {
        m_commandList->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
    }

    void GraphicsContext::SetPushConstants(uint32_t rootParameterIndex, uint32_t num32BitValues, const void* data, uint32_t destOffsetIn32BitValues)
    {
        // Inyecta valores crudos (ej. un índice, un color) directo en la firma sin crear buffers
        m_commandList->SetGraphicsRoot32BitConstants(rootParameterIndex, num32BitValues, data, destOffsetIn32BitValues);
    }

    void GraphicsContext::SetPushConstant(uint32_t rootParameterIndex, uint32_t sourceData,uint32_t destOffsetIn32BitValues)
    {
        m_commandList->SetGraphicsRoot32BitConstant(rootParameterIndex, sourceData, destOffsetIn32BitValues);
    }
}
