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
        
        GraphicsPipelineStateDesc psoDesc = {};
        // Definir el Input Layout (DEBE COINCIDIR CON ModelVertex Y CON EL HLSL)
        psoDesc.RootSignature = m_rootSignature.get();
        psoDesc.InputLayout = VertexPositionColor::GetLayout();

        psoDesc.VertexShaderDesc = { L"Shaders/UnlitColor.hlsl", L"VSMain", L"vs_6_0" };
        psoDesc.PixelShaderDesc = { L"Shaders/UnlitColor.hlsl", L"PSMain", L"ps_6_0" };
        
        psoDesc.RasterizerDesc.CullMode = CullMode::None;
    
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

    void DebugRenderer::DrawDirectionalLightGizmo(const Math::Vector3& position, const Math::Vector3& direction, float scale, const Math::Color& color)
    {
        Math::Vector3 dir = direction;
        dir.Normalize();
    
        // Punto final de la flecha principal
        Math::Vector3 endPos = position + (dir * scale);

        // 1. Dibujar el eje principal de la luz (El rayo de sol)
        m_primitiveBatch->DrawLine(position, endPos, color);

        // 2. Calcular ejes locales para dibujar la punta de la flecha
        // Evitamos el Gimbal Lock eligiendo un "Up" seguro
        Math::Vector3 up = (std::abs(dir.y) > 0.99f) ? Math::Vector3::Right : Math::Vector3::Up;
    
        Math::Vector3 right = dir.Cross(up);
        right.Normalize();
    
        up = right.Cross(dir);
        up.Normalize();

        // Tamaño de las "aletas" de la flecha
        float arrowHeadLength = scale * 0.2f;
        float arrowHeadWidth = scale * 0.1f;

        // Calcular los 4 puntos de la base de la pirámide de la punta
        Math::Vector3 baseCenter = endPos - (dir * arrowHeadLength);
        Math::Vector3 p1 = baseCenter + (up * arrowHeadWidth);
        Math::Vector3 p2 = baseCenter - (up * arrowHeadWidth);
        Math::Vector3 p3 = baseCenter + (right * arrowHeadWidth);
        Math::Vector3 p4 = baseCenter - (right * arrowHeadWidth);

        // Dibujar las 4 aletas conectando al punto final
        m_primitiveBatch->DrawLine(endPos, p1, color);
        m_primitiveBatch->DrawLine(endPos, p2, color);
        m_primitiveBatch->DrawLine(endPos, p3, color);
        m_primitiveBatch->DrawLine(endPos, p4, color);
    
        // 3. Dibujar un romboide 3D (octaedro) en el origen para identificar el emisor
        float emitterSize = scale * 0.15f; // Mantenemos la proporción atada al scale global del gizmo

        Math::Vector3 eTop    = position + up * emitterSize;
        Math::Vector3 eBottom = position - up * emitterSize;
        Math::Vector3 eRight  = position + right * emitterSize;
        Math::Vector3 eLeft   = position - right * emitterSize;
        Math::Vector3 eFront  = position + dir * emitterSize;
        Math::Vector3 eBack   = position - dir * emitterSize;

        // Anillo central (Ecuador)
        m_primitiveBatch->DrawLine(eTop, eRight, color);
        m_primitiveBatch->DrawLine(eRight, eBottom, color);
        m_primitiveBatch->DrawLine(eBottom, eLeft, color);
        m_primitiveBatch->DrawLine(eLeft, eTop, color);

        // Conectar a la punta delantera (hacia donde apunta la luz)
        m_primitiveBatch->DrawLine(eTop, eFront, color);
        m_primitiveBatch->DrawLine(eBottom, eFront, color);
        m_primitiveBatch->DrawLine(eRight, eFront, color);
        m_primitiveBatch->DrawLine(eLeft, eFront, color);

        // Conectar a la punta trasera
        m_primitiveBatch->DrawLine(eTop, eBack, color);
        m_primitiveBatch->DrawLine(eBottom, eBack, color);
        m_primitiveBatch->DrawLine(eRight, eBack, color);
        m_primitiveBatch->DrawLine(eLeft, eBack, color);
    }

    void DebugRenderer::RenderDirectionalLightGizmos(GraphicsContext* context, const Camera& camera, uint32_t frameIndex)
    {
        m_primitiveBatch->Begin(); 
    
        Math::Vector4 boxColor = { 0.0f, 1.0f, 0.0f, 1.0f };

        auto entities = m_scene->GetAllEntitiesWith<TransformComponent, DirectionalLightComponent>();
		
        for (auto entity : entities)
        {
            const auto& transform = entities.get<TransformComponent>(entity);
            const auto& directionalLight = entities.get<DirectionalLightComponent>(entity);

            const Math::Matrix& worldMatrix = transform.WorldTransform;

            DrawDirectionalLightGizmo(worldMatrix.Translation(), directionalLight.WorldDirection, 5.0f, Math::Color {1.0f, 0.0f, 0.0f});
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

            DirectX::BoundingBox localAABB
            (
                DirectX::XMFLOAT3(bbox.Center.x, bbox.Center.y, bbox.Center.z),
                DirectX::XMFLOAT3(bbox.Extents.x, bbox.Extents.y, bbox.Extents.z)
            );
            
            DirectX::BoundingOrientedBox obb;
            DirectX::BoundingOrientedBox::CreateFromBoundingBox(obb, localAABB);
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
