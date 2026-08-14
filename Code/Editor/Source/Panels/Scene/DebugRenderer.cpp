#include "brepch.h"
#include "DebugRenderer.h"

#include <DirectXCollision.h>

#include "Bruno/Platform/DirectX/GraphicsContext.h"
#include "Bruno/Platform/DirectX/GraphicsPipelineState.h"
#include "Bruno/Platform/DirectX/RootSignature.h"
#include "Bruno/Platform/DirectX/Shader.h"
#include "Bruno/Platform/DirectX/ShaderCompiler.h"
#include "Bruno/Platform/DirectX/VertexTypes.h"
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
        // 16 floats equivalen a una Matriz de 4x4
        CD3DX12_ROOT_PARAMETER gizmoParams[1];
        gizmoParams[0].InitAsConstants(16, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

        // Inicializamos la firma sin samplers
        m_rootSignature = std::make_unique<RootSignature>(*m_device);
        m_rootSignature->Initialize(1, gizmoParams);

        ShaderCompiler compiler; 

        auto vertexShaderByteCode = compiler.CompileFromFile(L"Shaders/UnlitColor.hlsl", L"VS", L"vs_6_0");
        auto pixelShaderByteCode  = compiler.CompileFromFile(L"Shaders/UnlitColor.hlsl", L"PS", L"ps_6_0");
		
        std::unique_ptr<ShaderProgram> vertexShader = std::make_unique<ShaderProgram>(ShaderStage::Vertex, vertexShaderByteCode);
        std::unique_ptr<ShaderProgram> pixelShader = std::make_unique<ShaderProgram>(ShaderStage::Pixel, pixelShaderByteCode);
        
        GraphicsPipelineStateDesc psoDesc = {};
        // Definir el Input Layout (DEBE COINCIDIR CON ModelVertex Y CON EL HLSL)
        psoDesc.RootSignature = m_rootSignature.get();
        psoDesc.InputLayout = VertexPositionColor::GetLayout();

        psoDesc.VertexShader = vertexShader.get();
        psoDesc.PixelShader = pixelShader.get();
        
        // 3. Rasterizer para Gizmos (Sin Culling para que siempre se vean)
        psoDesc.RasterizerState.CullMode = CullMode::None;
    
        psoDesc.DepthState.Mode = DepthMode::None;
        
        psoDesc.BlendState.Mode = BlendMode::AlphaBlend;
        
        psoDesc.Topology = PrimitiveTopology::LineList;

        //psoDesc.BlendState.RenderTarget[0] = blendDesc;
        
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = TextureFormat::R8G8B8A8_Unorm;
        psoDesc.DSVFormat = TextureFormat::D24_Unorm_S8_Uint;

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
        
        context->SetPrimitiveTopology(PrimitiveTopology::LineList);
        
        // Bind Root Signature y PSO
        context->SetRootSignature(m_rootSignature.get());
        context->SetPipelineState(m_psoDepthOff.get()); // Usar DepthOff si quieres que flote sobre todo
    
        // Bind Constantes (Root Constants o Constant Buffer temporal)
        GizmoConstants constants = { camera.GetViewProjection() };
        context->SetPushConstants(0, sizeof(GizmoConstants) / 4, &constants, 0);

        // Bind Buffers
        context->SetVertexBuffer(0, m_primitiveBatch->GetVertexBuffer(frameIndex));

        context->DrawInstanced(m_primitiveBatch->GetVertexCount(), 1, 0, 0);
    }
}
