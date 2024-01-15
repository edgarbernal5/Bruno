#include "brpch.h"
#include "SceneRenderer.h"

#include "Bruno/Scene/Scene.h"
#include "Bruno/Renderer/RenderItem.h"

#include "Bruno/Platform/DirectX/Texture.h"
#include "Bruno/Platform/DirectX/GraphicsContext.h"
#include "Bruno/Platform/DirectX/PipelineStateObject.h"
#include "Bruno/Platform/DirectX/RootSignature.h"
#include "Bruno/Platform/DirectX/Shader.h"
#include "Bruno/Platform/DirectX/Surface.h"

#include "Bruno/Content/AssetManager.h"

namespace Bruno
{
	SceneRenderer::SceneRenderer(std::shared_ptr<Scene> scene, Surface* surface, AbstractAssetManager* assetManager) :
		m_scene(scene),
		m_surface(surface),
		m_assetManager(assetManager)
	{
		m_opaqueShader = std::make_unique<Shader>(L"Shaders/Opaque.hlsl");

		PipelineResourceBinding textureBinding;
		textureBinding.BindingIndex = 0;

		m_meshPerObjectResourceSpace.SetCBV(scene->m_objectBuffer[0].get());
		m_meshPerObjectResourceSpace.SetSRV(textureBinding);
		m_meshPerObjectResourceSpace.Lock();

		PipelineResourceLayout meshResourceLayout;
		meshResourceLayout.Spaces[Graphics::Core::PER_OBJECT_SPACE] = &m_meshPerObjectResourceSpace;

		PipelineResourceMapping resourceMapping;
		m_rootSignature = std::make_shared<RootSignature>(meshResourceLayout, resourceMapping);

		GraphicsPipelineDesc meshPipelineDesc = GetDefaultGraphicsPipelineDesc();
		meshPipelineDesc.VertexShader = m_opaqueShader->GetShaderProgram(Shader::ShaderProgramType::Vertex);
		meshPipelineDesc.PixelShader = m_opaqueShader->GetShaderProgram(Shader::ShaderProgramType::Pixel);
		meshPipelineDesc.RenderTargetDesc.DepthStencilFormat = surface->GetDepthBufferFormat();
		meshPipelineDesc.RenderTargetDesc.RenderTargetsCount = 1;
		meshPipelineDesc.DepthStencilDesc.DepthEnable = true;
		meshPipelineDesc.RenderTargetDesc.RenderTargetFormats[0] = surface->GetSurfaceFormat();
		meshPipelineDesc.DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

		m_pipelineState = std::make_unique<PipelineStateObject>(meshPipelineDesc, m_rootSignature, resourceMapping);
	}

	void SceneRenderer::OnRender(GraphicsContext* graphicsContext)
	{
		auto device = Graphics::GetDevice();
		uint32_t frameIndex = device->GetFrameId();

		Texture& backBuffer = m_surface->GetBackBuffer();
		DepthBuffer& depthBuffer = m_surface->GetDepthBuffer();

		PipelineInfo pipeline;
		pipeline.Pipeline = m_pipelineState.get();
		pipeline.RenderTargets.push_back(&backBuffer);
		pipeline.DepthStencilTarget = &depthBuffer;
		graphicsContext->SetPipeline(pipeline);

		auto objectSize = AlignU32(sizeof(SceneObjectBuffer), 256);

		VertexBuffer* currentVB = nullptr;
		auto entities = m_scene->GetAllEntitiesWith<TransformComponent, ModelComponent>();
		uint32_t objectIndex = 0;
		for (auto& ent : entities)
		{
			auto [transformComponent, modelComponent] = entities.get<TransformComponent, ModelComponent>(ent);
			auto model = m_assetManager->GetAsset<Model>(modelComponent.ModelHandle);

			uint32_t meshIndex = modelComponent.MeshIndex;
			auto& meshes = model->GetMeshes();
			auto& mesh = meshes[meshIndex];
			
			auto textureHandle = model->GetMaterial(mesh->GetMaterialIndex())->TexturesByName["Texture"];
			auto texture = m_assetManager->GetAsset<Texture>(textureHandle);
			if (texture != nullptr && texture->IsReady())
			{
				if (!model->GetIndexBuffer()->IsReady() || !model->GetVertexBuffer()->IsReady())
					continue;

				if (currentVB != model->GetVertexBuffer().get())
				{
					graphicsContext->SetVertexBuffer(*model->GetVertexBuffer());
					graphicsContext->SetIndexBuffer(*model->GetIndexBuffer());
					currentVB = model->GetVertexBuffer().get();
				}

				PipelineResourceBinding textureBinding;
				textureBinding.BindingIndex = 0;
				textureBinding.Resource = texture.get();

				m_meshPerObjectResourceSpace.SetCBV(m_scene->m_objectBuffer[frameIndex].get(), objectIndex * objectSize);
				m_meshPerObjectResourceSpace.SetSRV(textureBinding);

				graphicsContext->SetPipelineResources(Graphics::Core::PER_OBJECT_SPACE, m_meshPerObjectResourceSpace);

				graphicsContext->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				graphicsContext->DrawIndexedInstanced(mesh->GetIndexCount(),
					1,
					mesh->GetBaseIndex(),
					mesh->GetBaseVertex(),
					0);
			}
			objectIndex++;
		}
	}
}