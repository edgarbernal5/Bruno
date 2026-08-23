#include "brpch.h"
#include "SceneRenderer.h"

#include "PrimitiveBatch.h"
#include "PSOCache.h"
#include "RootSignatureLibrary.h"
#include "Bruno/Scene/Scene.h"
#include "Bruno/Renderer/RenderItem.h"
#include "Bruno/Renderer/Material.h"

#include "Bruno/Platform/DirectX/Texture2D.h"
#include "Bruno/Platform/DirectX/GraphicsContext.h"
#include "Bruno/Platform/DirectX/RootSignature.h"
#include "Bruno/Platform/DirectX/Shader.h"

#include "Bruno/Content/AssetManager.h"
#include "Bruno/Core/Memory.h"
#include "Bruno/Core/ScopedCpuTimer.h"
#include "Bruno/Platform/DirectX/Profiler.h"
#include "Bruno/Platform/DirectX/VertexTypes.h"
#include "Bruno/Renderer/Camera.h"
#include "Bruno/Scene/Systems/FrustumCulling.h"

namespace Bruno
{
	SceneRenderer::SceneRenderer(std::shared_ptr<Scene> scene, std::shared_ptr<FrustumCulling> frustumCulling, AbstractAssetManager* assetManager) :
		m_scene(scene),
		m_frustumCulling(frustumCulling),
		m_assetManager(assetManager)
	{
		auto& device = Bruno::Graphics::GetDevice();
		
		InitializeOpaqueRootSignature(device);

		InitializeOpaquePSO(device);
	}

	void SceneRenderer::InitEntitiesForRender()
	{
		auto& device = Bruno::Graphics::GetDevice();
		
		size_t objectSize = AlignU32(sizeof(SceneObjectBuffer), 256);
		
		// Buscamos todas las entidades que tienen un Mesh y un Transform
		auto entities = m_scene->GetAllEntitiesWith<TransformComponent, ModelComponent>();
		for (auto& entt : entities)
		{
			Entity entity = { entt, m_scene.get() };
			
			// Si la entidad no tiene sus Constant Buffers, se los creamos
			if (!entity.HasComponent<CBVComponent>()) 
			{
				CBVComponent cbv;
				for (int i = 0; i < 2; ++i)
				{
					cbv.TransformCB[i] = std::make_shared<ConstantBuffer>(device, objectSize);
				}
                
				// Le "pegamos" el componente de memoria de video a la entidad
				entity.AddComponent<CBVComponent>(std::move(cbv));
			}
			const auto& modelComponent = entities.get<ModelComponent>(entt);
			uint32_t meshIndex = modelComponent.MeshIndex;
			auto model = m_assetManager->GetAsset<Model>(modelComponent.ModelHandle);
			auto& meshes = model->GetMeshes();
			auto& mesh = meshes[meshIndex];
			
			auto materialHandle = modelComponent.Materials->GetMaterial(mesh->GetMaterialIndex());
			auto material = m_assetManager->GetAsset<Material>(materialHandle);
			material->BuildDescriptors(device, &device->GetSRVDescriptorAllocator(), m_assetManager);
			
			material->SetPipelineState(m_opaquePSO, m_opaqueRootSignature);
		}
	}
	
	void SceneRenderer::InitializeOpaqueRootSignature(GraphicsDevice* device)
	{
		auto prototypeSig = std::make_shared<RootSignature>(*device);

		// Parámetro 0: Constant Buffer View en b0 (Matriz MVP)
		prototypeSig->AddConstantBufferView(0, 0, ShaderVisibility::Vertex);

		// Parámetro 1: Tabla de Descriptores para 1 textura en t0 
		prototypeSig->AddDescriptorTableSRV(1, 0, 0, ShaderVisibility::Pixel);

		// Sampler: Filtro lineal en s0
		prototypeSig->AddStaticSampler(
			0, 
			0, 
			TextureFilter::Linear, 
			TextureAddressMode::Wrap, 
			ShaderVisibility::Pixel
		);

		m_opaqueRootSignature = RootSignatureLibrary::GetOrCreate(prototypeSig);
	}

	void SceneRenderer::InitializeOpaquePSO(GraphicsDevice* device)
	{
		GraphicsPipelineStateDesc psoDesc = {};
		// Definir el Input Layout (DEBE COINCIDIR CON ModelVertex Y CON EL HLSL)
		psoDesc.RootSignature = m_opaqueRootSignature.get();
		psoDesc.InputLayout = VertexPositionNormalTexture::GetLayout();
		
        psoDesc.VertexShaderDesc = { L"Shaders/Opaque.hlsl", L"VSMain", L"vs_6_0" };
        psoDesc.PixelShaderDesc = { L"Shaders/Opaque.hlsl", L"PSMain", L"ps_6_0" };
		
		psoDesc.RasterizerState.CullMode = CullMode::Back;
		psoDesc.RasterizerState.FillMode = FillMode::Solid;
		psoDesc.RasterizerState.FrontCounterClockwise = true;
        
        psoDesc.Topology = PrimitiveTopology::TriangleList;
    
        // Formatos de Salida (DEBEN coincidir con tu SwapChain y DepthBuffer)
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = TextureFormat::R8G8B8A8_Unorm;
        psoDesc.DSVFormat = TextureFormat::D24_Unorm_S8_Uint;
		
		m_opaquePSO = PSOCache::GetOrCreate(device, psoDesc);
	}

	void SceneRenderer::RenderScene(GraphicsContext* graphicsContext, Camera& camera, uint32_t frameIndex)
	{
		ScopedCpuTimer totalCpuTimer(&Profiler::Get().Stats.CpuTotalRenderTimeMs);
		
		Profiler::Get().Stats.ResetCounters();
		
		ID3D12GraphicsCommandList* cmdList = graphicsContext->GetNative();
		{
			ScopedCpuTimer cullingTimer(&Profiler::Get().Stats.CpuCullingTimeMs);
			m_frustumCulling->Update();
			Profiler::Get().Stats.TotalEntities = m_frustumCulling->GetTotalEntites();
			Profiler::Get().Stats.RenderedEntities = m_frustumCulling->GetTotalVisibleEntities();
		}
		auto& visibleEntities = m_frustumCulling->GetVisibleEntities();
		
		VertexBuffer* currentVB = nullptr;
		GraphicsPipelineState* currentPSO = nullptr;
		
		// 2. --- RENDERIZADO GPU ---
		Profiler::Get().StartGpuTimer(cmdList);
		
		for (Entity entity : visibleEntities)
		{
			const auto& modelComponent = entity.GetComponent<ModelComponent>();
			const auto& cbv = entity.GetComponent<CBVComponent>();
			
			auto model = m_assetManager->GetAsset<Model>(modelComponent.ModelHandle);

			uint32_t meshIndex = modelComponent.MeshIndex;
			auto& meshes = model->GetMeshes();
			auto& mesh = meshes[meshIndex];
			
			auto materialHandle = modelComponent.Materials->GetMaterial(mesh->GetMaterialIndex());
			auto material = m_assetManager->GetAsset<Material>(materialHandle);
			
			AssetHandle textureHandle{ 0 };
			
			if (material)
			{
				auto textIt = material->TexturesByName.find("Texture");
				if (textIt != material->TexturesByName.end())
				{
					textureHandle = textIt->second;
				}
			}
			
			auto texture = m_assetManager->GetAsset<Texture2D>(textureHandle);
			if (texture != nullptr)
			{
				auto& indexBuffer = model->GetIndexBuffer();
				auto& vertexBuffer = model->GetVertexBuffer();
				if (currentVB != vertexBuffer.get())
				{
					graphicsContext->SetVertexBuffer(0, vertexBuffer.get());
					graphicsContext->SetIndexBuffer(indexBuffer.get());
					currentVB = vertexBuffer.get();
				}
				
				if (currentPSO != material->GetPSO().get())
				{
					graphicsContext->SetPipelineState(material->GetPSO().get());
					currentPSO = material->GetPSO().get();
				}
				graphicsContext->SetRootSignature(material->GetRootSignature().get());
				
				// Enlazar la tabla de texturas (Parámetro 1 en nuestra Root Signature)
				graphicsContext->SetDescriptorTable(1, material->GetTextureDescriptorTable());
				
				Math::Matrix world = m_scene->GetWorldSpaceMatrix(entity);
				Math::Matrix wvp = (world * camera.GetViewProjection()).Transpose();
				
				SceneObjectBuffer objConstants;
				objConstants.WorldViewProjection = wvp;
				cbv.TransformCB[frameIndex]->Update(&objConstants, sizeof(SceneObjectBuffer));
				
				graphicsContext->SetConstantBuffer(0, cbv.TransformCB[frameIndex].get());
				graphicsContext->SetPrimitiveTopology(PrimitiveTopology::TriangleList);
				graphicsContext->DrawIndexedInstanced(mesh->GetIndexCount(),
					1,
					mesh->GetBaseIndex(),
					mesh->GetBaseVertex(),
					0);
				
				Profiler::Get().Stats.DrawCalls++;
				Profiler::Get().Stats.TriangleCount += (mesh->GetIndexCount() / 3);
			}
		}
		
		Profiler::Get().StopGpuTimer(cmdList);
		
		// Le ordenamos a la GPU copiar los tiempos al buffer leíble
		Profiler::Get().ResolveGpuTimestamps(cmdList);
	}

}
