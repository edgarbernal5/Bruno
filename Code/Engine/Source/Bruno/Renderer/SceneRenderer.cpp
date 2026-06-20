#include "brpch.h"
#include "SceneRenderer.h"

#include "Bruno/Scene/Scene.h"
#include "Bruno/Renderer/RenderItem.h"
#include "Bruno/Renderer/Material.h"

#include "Bruno/Platform/DirectX/Texture.h"
#include "Bruno/Platform/DirectX/GraphicsContext.h"
#include "Bruno/Platform/DirectX/PipelineStateObject.h"
#include "Bruno/Platform/DirectX/RootSignature.h"
#include "Bruno/Platform/DirectX/Shader.h"
#include "Bruno/Platform/DirectX/Surface_Gem.h"

#include "Bruno/Content/AssetManager.h"
#include "Bruno/Platform/DirectX/ShaderCompiler.h"
#include "Bruno/Platform/DirectX/Texture2D.h"

namespace Bruno
{
	SceneRenderer::SceneRenderer(std::shared_ptr<Scene> scene, AbstractAssetManager* assetManager) :
		m_scene(scene),
		m_assetManager(assetManager)
	{
		auto& device = Bruno::Graphics::GetDXDevice();
		
		DX::ShaderCompiler compiler; 

		// Compilas usando DXC (nota el _6_0)
		auto vertexShaderByteCode = compiler.CompileFromFile(L"Shaders/Opaque.hlsl", L"VS", L"vs_6_0");
		auto pixelShaderByteCode  = compiler.CompileFromFile(L"Shaders/Opaque.hlsl", L"PS", L"ps_6_0");
		
		m_opaqueRootSignature = std::make_shared<DX::RootSignature>(*device);
		m_opaqueRootSignature->CreateOpaqueSignature();

		// Instanciamos el Pipeline State Object (PSO) pasándole el contrato y shaders
		m_opaquePSO = std::make_unique<DX::GraphicsPipelineState>(*device);
		m_opaquePSO->CreateOpaquePSO(
			m_opaqueRootSignature->GetNative(), 
			vertexShaderByteCode.Get(), 
			pixelShaderByteCode.Get()
		);
		
		/*PipelineResourceBinding textureBinding;
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

		m_pipelineState = std::make_unique<PipelineStateObject>(meshPipelineDesc, m_rootSignature, resourceMapping);*/
	}

	void SceneRenderer::InitEntitiesForRender()
	{
		auto& device = Bruno::Graphics::GetDXDevice();
		
		size_t objectSize = AlignU32(sizeof(SceneObjectBuffer), 256);
		// Buscamos todas las entidades que tienen un Mesh y un Transform
		auto entities = m_scene->GetAllEntitiesWith<TransformComponent, ModelComponent>();
		for (auto& ent : entities)
		{
			Entity entity = { ent, m_scene.get() };
			
			// Si la entidad no tiene sus Constant Buffers, se los creamos
			if (!entity.HasComponent<CBVComponent>()) 
			{
				CBVComponent cbv;
				for (int i = 0; i < 2; ++i)
				{
					cbv.TransformCB[i] = std::make_shared<DX::ConstantBuffer>(device->GetNativeDevice().Get(), objectSize);
				}
                
				// Le "pegamos" el componente de memoria de video a la entidad
				entity.AddComponent<CBVComponent>(std::move(cbv));
			}
		}
	}

	void SceneRenderer::OnRender(GraphicsContext* graphicsContext)
	{
		//auto device = Graphics::GetDevice();
		//uint32_t frameIndex = device->GetFrameId();

		//Texture& backBuffer = m_surface->GetBackBuffer();
		//DepthBuffer& depthBuffer = m_surface->GetDepthBuffer();

		
		/*PipelineInfo pipeline;
		pipeline.Pipeline = m_pipelineState.get();
		pipeline.RenderTargets.push_back(&backBuffer);
		pipeline.DepthStencilTarget = &depthBuffer;
		graphicsContext->SetPipeline(pipeline);

		auto objectSize = AlignU32(sizeof(SceneObjectBuffer), 256);

		VertexBuffer* currentVB = nullptr;
		uint32_t objectIndex = 0;*/
		auto entities = m_scene->GetAllEntitiesWith<TransformComponent, ModelComponent, CBVComponent>();
		for (auto& ent : entities)
		{
			const auto& [transformComponent, modelComponent, cbv] = entities.get<TransformComponent, ModelComponent, CBVComponent>(ent);
			auto model = m_assetManager->GetAsset<Model>(modelComponent.ModelHandle);

			uint32_t meshIndex = modelComponent.MeshIndex;
			auto& meshes = model->GetMeshes();
			auto& mesh = meshes[meshIndex];
			
			auto materialHandle = modelComponent.Materials->GetMaterial(mesh->GetMaterialIndex());
			auto material = m_assetManager->GetAsset<Material>(materialHandle);
			AssetHandle textureHandle{ 0 };
			auto textIt = material->TexturesByName.find("Texture");
			if (textIt != material->TexturesByName.end())
			{
				textureHandle = textIt->second;
			}
			auto texture = m_assetManager->GetAsset<DX::Texture2D>(textureHandle);
			if (texture != nullptr)
			{
				
			}
			/*if (texture != nullptr && texture->IsReady())
			{
				auto& indexBuffer = model->GetIndexBuffer();
				auto& vertexBuffer = model->GetVertexBuffer();
				if (!indexBuffer->IsReady() || !vertexBuffer->IsReady())
					continue;

				if (currentVB != vertexBuffer.get())
				{
					graphicsContext->SetVertexBuffer(*vertexBuffer);
					graphicsContext->SetIndexBuffer(*indexBuffer);
					currentVB = vertexBuffer.get();
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
			objectIndex++;*/
		}
	}
}
