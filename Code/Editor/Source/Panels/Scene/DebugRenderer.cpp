#include "brepch.h"
#include "DebugRenderer.h"

#include <DirectXCollision.h>

#include "Bruno/Platform/DirectX/GraphicsContext.h"
#include "Bruno/Platform/DirectX/GraphicsPipelineState.h"
#include "Bruno/Platform/DirectX/RootSignature.h"
#include "Bruno/Platform/DirectX/ShaderCompiler.h"
#include "Bruno/Renderer/PrimitiveBatch.h"
#include "Bruno/Scene/Components.h"
#include "Bruno/Scene/Scene.h"
#include "Gizmos/GizmoBasicTypes.h"

namespace Bruno
{
    DebugRenderer::DebugRenderer(GraphicsDevice* device, std::shared_ptr<Scene> scene) : 
        m_device(device),
        m_scene(scene)
    {
        ShaderCompiler compiler; 

        // Compilas usando DXC (nota el _6_0)
        auto vertexShaderByteCode = compiler.CompileFromFile(L"Shaders/UnlitColor.hlsl", L"VS", L"vs_6_0");
        auto pixelShaderByteCode  = compiler.CompileFromFile(L"Shaders/UnlitColor.hlsl", L"PS", L"ps_6_0");
		
        // 16 floats equivalen a una Matriz de 4x4
        CD3DX12_ROOT_PARAMETER gizmoParams[1];
        gizmoParams[0].InitAsConstants(16, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

        // Inicializamos la firma sin samplers
        m_rootSignature = std::make_unique<RootSignature>(*m_device);
        m_rootSignature->Initialize(1, gizmoParams);

        // 1. Input Layout EXCLUSIVO para Gizmos (Position + Color)
        D3D12_INPUT_ELEMENT_DESC inputLayout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        // 2. Llenar el descriptor
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputLayout, _countof(inputLayout) };
        psoDesc.pRootSignature = m_rootSignature->GetNative();
        psoDesc.VS = { reinterpret_cast<BYTE*>(vertexShaderByteCode->GetBufferPointer()), vertexShaderByteCode->GetBufferSize() };
        psoDesc.PS = { reinterpret_cast<BYTE*>(pixelShaderByteCode->GetBufferPointer()), pixelShaderByteCode->GetBufferSize() };
        
        // 3. Rasterizer para Gizmos (Sin Culling para que siempre se vean)
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // CRÍTICO para primitivas sueltas
    
        // 4. Depth Stencil para "Rayos X" o Gizmos sobrepuestos
        D3D12_DEPTH_STENCIL_DESC depthDesc = {};
        depthDesc.DepthEnable = TRUE;
        depthDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO; // No escriben en el Z-Buffer
        depthDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
        psoDesc.DepthStencilState = depthDesc;
        
        D3D12_RENDER_TARGET_BLEND_DESC blendDesc = {};
        blendDesc.BlendEnable = TRUE;
        blendDesc.LogicOpEnable = FALSE;
        // El color del anillo se multiplica por su propio Alpha (0.15)
        blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA; 
        // El color del fondo se multiplica por (1.0 - 0.15 = 0.85)
        blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA; 
        blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
        // El canal alfa en sí (opcional dependiendo de si compones a otra textura)
        blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
        blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
        blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
        blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

        psoDesc.BlendState.RenderTarget[0] = blendDesc;
        
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
        psoDesc.SampleDesc.Count = 1;

        // 5. Instanciar y configurar la clase genérica
        m_psoDepthOff = std::make_unique<GraphicsPipelineState>(*m_device);
        m_psoDepthOff->Initialize(psoDesc);
        
        m_primitiveBatch = std::make_unique<PrimitiveBatch>(m_device);
    }

    void DebugRenderer::RenderBoundingBoxes(GraphicsContext* context, const Camera& camera, uint32_t frameIndex)
    {
        // 1. Inicias tu Batch para este frame
        m_primitiveBatch->Begin(); 
    
        // Color de debug (Ej: Verde neón)
        Math::Vector4 boxColor = { 0.0f, 1.0f, 0.0f, 1.0f };

        // 2. Iteras todas las entidades que tienen Transform y Bounding Box
        auto entities = m_scene->GetAllEntitiesWith<TransformComponent, BoundingBoxComponent>();
		
        for (auto entity : entities)
        {
            const auto& transform = entities.get<TransformComponent>(entity);
            const auto& bbox = entities.get<BoundingBoxComponent>(entity);

            // 3. Obtener la matriz de mundo de la entidad
            const Math::Matrix& worldMatrix = transform.WorldTransform;

            DirectX::BoundingBox localAABB(
                DirectX::XMFLOAT3(bbox.Center.x, bbox.Center.y, bbox.Center.z),
                DirectX::XMFLOAT3(bbox.Extents.x, bbox.Extents.y, bbox.Extents.z)
                );
            
            // 4. Transformar el AABB local a un OBB mundial
            DirectX::BoundingOrientedBox obb;
            DirectX::BoundingOrientedBox::CreateFromBoundingBox(obb,localAABB);
            obb.Transform(obb, worldMatrix); 

            // 5. Enviar al Batcher
            m_primitiveBatch->DrawWireBox(obb, boxColor);
        }
        
        // 6. Terminas y emites los Draw Calls
        m_primitiveBatch->End(frameIndex);
        
        context->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
        
        // Bind Root Signature y PSO
        context->SetRootSignature(m_rootSignature.get()->GetNative());
        context->SetPipelineState(m_psoDepthOff.get()->GetNative()); // Usar DepthOff si quieres que flote sobre todo
    
        // Bind Constantes (Root Constants o Constant Buffer temporal)
        GizmoConstants constants = { camera.GetViewProjection() };
        context->SetGraphicsRoot32BitConstants(0, sizeof(GizmoConstants) / 4, &constants, 0);

        // Bind Buffers
        context->SetVertexBuffer(m_primitiveBatch->GetVertexBuffer(frameIndex)->GetView());

        context->DrawInstanced(m_primitiveBatch->GetVertexCount(), 1, 0, 0);
    }
}
