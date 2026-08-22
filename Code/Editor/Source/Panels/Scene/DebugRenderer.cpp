#include "brepch.h"
#include "DebugRenderer.h"

#include <DirectXCollision.h>

#include "Bruno/Platform/DirectX/GraphicsContext.h"
#include "Bruno/Platform/DirectX/GraphicsPipelineState.h"
#include "Bruno/Platform/DirectX/RootSignature.h"
#include "Bruno/Platform/DirectX/Shader.h"
#include "Bruno/Platform/DirectX/VertexTypes.h"
#include "Bruno/Renderer/PrimitiveBatch.h"
#include "Bruno/Renderer/RootSignatureLibrary.h"
#include "Bruno/Renderer/ShaderLibrary.h"
#include "Bruno/Scene/Components.h"
#include "Bruno/Scene/Scene.h"
#include "Gizmos/GizmoBasicTypes.h"

namespace Bruno
{
    DebugRenderer::DebugRenderer(GraphicsDevice* device, std::shared_ptr<Scene> scene) : 
        m_device(device),
        m_scene(scene)
    {
        auto prototypeSig = std::make_shared<RootSignature>(*m_device);
        prototypeSig->AddConstants(sizeof(Math::Matrix) / 4, 0, 0, ShaderVisibility::Vertex);
        
        m_rootSignature = RootSignatureLibrary::GetOrCreate(prototypeSig);
        
        ShaderCompileDesc vsDesc = { L"Shaders/UnlitColor.hlsl", L"VSMain", L"vs_6_0" };
        auto vertexShader = ShaderLibrary::GetOrCompile(vsDesc);
        
        ShaderCompileDesc psDesc = { L"Shaders/UnlitColor.hlsl", L"PSMain", L"ps_6_0" };
        auto pixelShader = ShaderLibrary::GetOrCompile(psDesc);
        
        GraphicsPipelineStateDesc psoDesc = {};
        // Definir el Input Layout (DEBE COINCIDIR CON ModelVertex Y CON EL HLSL)
        psoDesc.RootSignature = m_rootSignature.get();
        psoDesc.InputLayout = VertexPositionColor::GetLayout();

        psoDesc.VertexShaderDesc = vsDesc;
        psoDesc.PixelShaderDesc = psDesc;
        
        psoDesc.RasterizerState.CullMode = CullMode::None;
    
        psoDesc.DepthState.Mode = DepthMode::ReadOnly;
        
        psoDesc.BlendState.Mode = BlendMode::AlphaBlend;
        
        psoDesc.Topology = PrimitiveTopology::LineList;
        
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = TextureFormat::R8G8B8A8_Unorm;
        psoDesc.DSVFormat = TextureFormat::D24_Unorm_S8_Uint;

        m_psoDepthOff = std::make_unique<GraphicsPipelineState>(*m_device);
        m_psoDepthOff->Initialize(psoDesc);
        
        m_primitiveBatch = std::make_unique<PrimitiveBatch>(m_device);
    }

    void DebugRenderer::RenderBoundingBoxes(GraphicsContext* context, const Camera& camera, uint32_t frameIndex)
    {
        m_primitiveBatch->Begin(); 
    
        Math::Vector4 boxColor = { 0.0f, 1.0f, 0.0f, 1.0f };

        auto entities = m_scene->GetAllEntitiesWith<TransformComponent, BoundingBoxComponent>();
		
        for (auto entity : entities)
        {
            const auto& transform = entities.get<TransformComponent>(entity);
            const auto& bbox = entities.get<BoundingBoxComponent>(entity);

            const Math::Matrix& worldMatrix = transform.WorldTransform;

            DirectX::BoundingBox localAABB(
                DirectX::XMFLOAT3(bbox.Center.x, bbox.Center.y, bbox.Center.z),
                DirectX::XMFLOAT3(bbox.Extents.x, bbox.Extents.y, bbox.Extents.z)
                );
            
            DirectX::BoundingOrientedBox obb;
            DirectX::BoundingOrientedBox::CreateFromBoundingBox(obb,localAABB);
            obb.Transform(obb, worldMatrix); 

            m_primitiveBatch->DrawWireBox(obb, boxColor);
        }
        
        m_primitiveBatch->End(frameIndex);
        
        context->SetPrimitiveTopology(PrimitiveTopology::LineList);
        
        context->SetRootSignature(m_rootSignature.get());
        context->SetPipelineState(m_psoDepthOff.get());
        
        GizmoConstants constants = { camera.GetViewProjection() };
        context->SetPushConstants(0, sizeof(GizmoConstants) / 4, &constants, 0);

        // Bind Buffers
        context->SetVertexBuffer(0, m_primitiveBatch->GetVertexBuffer(frameIndex));

        context->DrawInstanced(m_primitiveBatch->GetVertexCount(), 1, 0, 0);
    }
}
