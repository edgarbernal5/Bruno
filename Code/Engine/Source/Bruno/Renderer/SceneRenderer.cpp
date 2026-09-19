#include "brpch.h"
#include "SceneRenderer.h"

#include "Bruno/Renderer/MaterialData.h"
#include "Bruno/Renderer/MaterialManager.h"
#include "Model.h"
#include "PrimitiveBatch.h"
#include "PSOCache.h"
#include "RootSignatureLibrary.h"
#include "Bruno/Scene/Scene.h"

#include "Bruno/Platform/DirectX/DepthBuffer.h"
#include "Bruno/Renderer/Material.h"

#include "Bruno/Platform/DirectX/Texture2D.h"
#include "Bruno/Platform/DirectX/GraphicsContext.h"
#include "Bruno/Platform/DirectX/RootSignature.h"
#include "Bruno/Platform/DirectX/Shader.h"

#include "Bruno/Content/AssetManager.h"
#include "Bruno/Core/JobSystem.h"
#include "Bruno/Core/Memory.h"
#include "Bruno/Core/ScopedCpuTimer.h"
#include "Bruno/Platform/DirectX/DynamicAllocation.h"
#include "Bruno/Platform/DirectX/Profiler.h"
#include "Bruno/Platform/DirectX/VertexTypes.h"
#include "Bruno/Renderer/Camera.h"
#include "Bruno/Scene/Systems/CullingSystem.h"
#include "Shadows/CascadedShadows.h"


namespace Bruno
{
	SceneRenderer::SceneRenderer(std::shared_ptr<Scene> scene, std::shared_ptr<CullingSystem> frustumCulling, AbstractAssetManager* assetManager) :
		m_scene(scene),
		m_frustumCulling(frustumCulling),
		m_assetManager(assetManager)
	{
		auto& device = Graphics::GetDevice();
		
		m_globalSrvHeap = &device->GetSRVDescriptorAllocator();
		
		InitializeShadowPipeline(device);
		
		m_materialManager = std::make_unique<MaterialManager>(*device, device->GetSRVDescriptorAllocator());
	}
	
	SceneRenderer::~SceneRenderer() = default;

	void SceneRenderer::InitEntitiesForRender()
	{
		auto entities = m_scene->GetAllEntitiesWith<TransformComponent, ModelComponent>();
		for (auto& entt : entities)
		{
			auto& modelComponent = entities.get<ModelComponent>(entt);
			uint32_t meshIndex = modelComponent.MeshIndex;
			auto model = m_assetManager->GetAsset<Model>(modelComponent.ModelHandle);
			auto& meshes = model->GetMeshes();
			auto& mesh = meshes[meshIndex];
			
			auto materialHandle= modelComponent.Materials->GetMaterial(mesh->GetMaterialIndex());
			if (materialHandle != 0)
			{
				// Le pedimos el material real al AssetManager
				std::shared_ptr<Material> materialAsset = m_assetManager->GetAsset<Material>(materialHandle);
            
				if (materialAsset)
				{
					RegisterMaterialToGPU(materialAsset);
					modelComponent.RuntimeMaterialIndex = materialAsset->RuntimeMaterialIndex;
				}
			}
		}
	}
	
	void SceneRenderer::RenderDeferred(GraphicsContext* context, Camera& camera, uint32_t frameIndex)
	{
		// ==========================================
		// FASE 2: G-BUFFER PASS (Geometría Opaca)
		// ==========================================
		/*context->TransitionResource(m_gBuffer->GetAlbedoMetalness(), ResourceState::PixelShaderResource, ResourceState::RenderTarget);
		// ... transicionar Normal y Position también ...

		context->SetPipelineState(m_gbufferPSO.get());
		context->SetRootSignature(m_gbufferRootSig.get());

		// Enlazar los 3 RTVs y el DSV principal
		Texture2D* gbufferTargets[] = { 
			m_gBuffer->GetAlbedoMetalness(), 
			m_gBuffer->GetNormalRoughness(), 
			m_gBuffer->GetPosition() 
		};
		context->SetRenderTargets(3, gbufferTargets, m_gBuffer->GetDepth());
		
		// Limpiar G-Buffer y Restaurar Viewport de pantalla
		context->SetViewport(0, 0, 100, 100);
    
		m_cameraCB.Update(*context, m_camera.GetViewProj());
		context->SetConstantBuffer(0, m_cameraCB);

		DrawBatch(context, registry, m_cullingResults.VisibleEntities);
		
		// ==========================================
		// FASE 3: DEFERRED LIGHTING PASS
		// ==========================================
		// Retornar recursos a estado de lectura
		context->TransitionResource(m_gBuffer->GetAlbedoMetalness()->GetResource(), ResourceState::RenderTarget, ResourceState::PixelShaderResource);
		context->TransitionResource(m_shadowMapArray->GetResource(), ResourceState::DepthWrite, ResourceState::PixelShaderResource);

		context->SetPipelineState(m_deferredLightingPSO.get());
		context->SetRootSignature(m_deferredLightingRootSig.get());

		// Enlazar el SwapChain (Pantalla final)
		auto backBuffer = m_surface->GetCurrentRenderTarget();
		context->SetRenderTargets(1, &backBuffer, nullptr);

		// Enlazar el Mega Heap Global y las constantes de iluminación
		context->SetDescriptorHeaps(m_globalSrvHeap.get());
    
		LightData cbData;
		// Rellenar cbData con LightViewProj, CascadeSplits, LightDir, etc.
		m_lightCB.Update(*context, cbData);
		context->SetConstantBuffer(0, m_lightCB);

		// Disparar el triángulo a pantalla completa sin Vertex Buffer
		context->DrawInstanced(3, 1, 0, 0);*/
		
	}

	void SceneRenderer::Resize(uint32_t width, uint32_t height)
	{
		//m_gBuffer->Resize()
	}
	
	void SceneRenderer::ExecuteMassiveCulling(const Camera& camera)
	{
		/*
		// 1. Frustum de Cámara Principal
		DirectX::BoundingFrustum cameraFrustum(camera.GetProjection());
		cameraFrustum.Transform(cameraFrustum, camera.GetViewInverse());

		DirectX::BoundingOrientedBox cascadeOBBs[NUM_CASCADES];
		for (uint32_t i = 0; i < NUM_CASCADES; ++i)
		{
			// 2. Convertimos tu matriz de cascada en una caja 3D sólida
			cascadeOBBs[i] = CreateOBBFromOrthographicMatrix(cascades[i].LightViewProj);
		}

		//auto group = registry.group<BoundingBoxComponent>(entt::get<TransformComponent>);
		//const entt::entity* entities = group.data();
		auto entitiesGroup = m_scene->GetAllEntitiesWith<TransformComponent, BoundingBoxComponent>();
		std::vector<entt::entity> entities(entitiesGroup.begin(), entitiesGroup.end());
		const size_t entityCount = entities.size();
		if (entityCount == 0)
		{
			return;
		}

		const uint32_t chunkSize = 1024;
		const uint32_t numChunks = (entityCount + chunkSize - 1) / chunkSize;
    
		// Resize y Clear rápido de los vectores pre-alojados (Omitido por brevedad, igual que antes)
		PrepareCullingChunks(numChunks, chunkSize);

		JobDispatchGroup cullingGroup;

		// ==========================================
		// FASE MAP: Evaluamos TODO en un solo barrido de caché
		// ==========================================
		JobSystem::Get().Dispatch(entityCount, chunkSize, [&](uint32_t start, uint32_t end) {
			uint32_t chunkIndex = start / chunkSize;
			auto& localResult = m_cullingChunks[chunkIndex];

			for (uint32_t i = start; i < end; ++i)
			{
				entt::entity entity = entities[i];
				const auto& [transform, bbox] = entitiesGroup.get<TransformComponent, BoundingBoxComponent>(entity);
				
				DirectX::BoundingBox localAABB(
				DirectX::XMFLOAT3(bbox.Center.x, bbox.Center.y, bbox.Center.z),
				DirectX::XMFLOAT3(bbox.Extents.x, bbox.Extents.y, bbox.Extents.z)
				);
            
				DirectX::BoundingOrientedBox worldOBB;
				DirectX::BoundingOrientedBox::CreateFromBoundingBox(worldOBB, localAABB);
				worldOBB.Transform(worldOBB, transform.WorldTransform); 

				// A. ¿Es visible por la cámara principal?
				if (cameraFrustum.Intersects(worldOBB))
				{
					localResult.VisibleEntities.push_back(entity);
				}

				// B. ¿Proyecta sombra en alguna cascada?
				for (uint32_t c = 0; c < NUM_CASCADES; ++c)
				{
					// Intersección Caja contra Caja (SIMD Ultra rápido)
					if (cascadeOBBs[c].Intersects(worldOBB))
					{
						localResult.ShadowCascades[c].push_back(entity);
					}
				}
			}
		}, &cullingGroup);

		// Espera Activa
		JobSystem::Get().Wait(cullingGroup);

		// ==========================================
		// FASE REDUCE: Consolidamos TODAS las listas
		// ==========================================
		ConsolidateFinalLists(numChunks);*/
	}
	
	void SceneRenderer::InitializeShadowPipeline(GraphicsDevice* device)
	{
		auto prototypeSig = std::make_shared<RootSignature>(*device);
		
		// b0: ShadowConstants (Contiene World y LightViewProj)
		prototypeSig->AddConstantBufferView(0, 0, ShaderVisibility::Vertex);
    
		m_shadowRootSig = RootSignatureLibrary::GetOrCreate(prototypeSig);
		
		GraphicsPipelineStateDesc shadowDesc = {};
		shadowDesc.RootSignature = m_shadowRootSig.get();
    
		// Solo cargamos el Vertex Shader. El Pixel Shader queda en nullptr/vacío.
		shadowDesc.VertexShaderDesc = { L"Shaders/ShadowMaps.hlsl", L"VSMain", L"vs_6_0" };
    
		shadowDesc.InputLayout = VertexPosition::GetLayout();
		shadowDesc.Topology = PrimitiveTopology::TriangleList;
    
		// ¡CRÍTICO! Cero Render Targets, solo usamos el Depth Stencil
		shadowDesc.NumRenderTargets = 0;
		shadowDesc.DSVFormat = TextureFormat::D32_Float;
    
		shadowDesc.DepthState.Mode = DepthMode::ReadWrite;
		shadowDesc.DepthState.Func = ComparisonFunc::Less;

		// Ajustes AAA: Hardware Depth Bias
		// Desplaza ligeramente la geometría hacia atrás desde el punto de vista de la luz
		shadowDesc.RasterizerDesc.DepthBias = 100000;         // Unidades internas
		shadowDesc.RasterizerDesc.DepthBiasClamp = 0.0f;
		shadowDesc.RasterizerDesc.SlopeScaledDepthBias = 1.5f; // Mayor inclinación = Mayor Bias

		m_shadowPSO = PSOCache::GetOrCreate(device, shadowDesc);
	}

	void SceneRenderer::RegisterMaterialToGPU(std::shared_ptr<Material> matAsset)
	{
		// Si ya tiene un índice válido en caché, lo ignoramos
		if (matAsset->RuntimeMaterialIndex != 0xFFFFFFFF) 
		{
			return; 
		}

		// 1. Creamos la estructura alineada para la GPU
		MaterialData gpuData = {};
		gpuData.AlbedoTint = matAsset->AlbedoTint;
		gpuData.MetallicFactor = matAsset->MetallicFactor;
		gpuData.RoughnessFactor = matAsset->RoughnessFactor;

		// 2. Resolvemos las texturas reales buscando en tu AssetManager
		if (matAsset->AlbedoMap != 0) 
		{
			auto tex = m_assetManager->GetAsset<Texture2D>(matAsset->AlbedoMap);
			// Obtenemos el índice Bindless que la textura guardó al nacer en el Mega Heap
			gpuData.AlbedoTextureIndex = tex->GetBindlessIndex();
		}
		else 
		{
			gpuData.AlbedoTextureIndex = 0xFFFFFFFF;
		}
		
		if (matAsset->NormalMap != 0) 
		{
			auto tex = m_assetManager->GetAsset<Texture2D>(matAsset->NormalMap);
			gpuData.NormalTextureIndex = tex->GetBindlessIndex();
		}
		else 
		{
			gpuData.NormalTextureIndex = 0xFFFFFFFF;
		}

		// 3. Enviamos esta data al MaterialManager global y guardamos la llave
		matAsset->RuntimeMaterialIndex = m_materialManager->CreateMaterial(gpuData);
	}

	void SceneRenderer::DrawBatch(GraphicsContext* graphicsContext, const std::vector<entt::entity>& visibleEntities)
	{
		if (visibleEntities.empty())
		{
			return;
		}
		
		auto entities = m_scene->GetAllEntitiesWith<TransformComponent, ModelComponent>();
		
		AssetHandle currentModel = 0;
		for (entt::entity entity : visibleEntities)
		{
			const auto& transform = entities.get<TransformComponent>(entity);
			const auto& modelComp = entities.get<ModelComponent>(entity);

			if (modelComp.RuntimeMaterialIndex == 0xFFFFFFFF)
			{
				continue;
			}
			
			auto model = m_assetManager->GetAsset<Model>(modelComp.ModelHandle);
			uint32_t meshIndex = modelComp.MeshIndex;
			auto& meshes = model->GetMeshes();
			auto& mesh = meshes[meshIndex];
			
			graphicsContext->SetPushConstants(2, modelComp.RuntimeMaterialIndex, 0);
			
			auto& indexBuffer = model->GetIndexBuffer();
			auto& vertexBuffer = model->GetVertexBuffer();
			
			// 4. Optimización de Estado: Solo re-enlazar geometría si cambiamos de asset
			if (modelComp.ModelHandle != currentModel)
			{
				graphicsContext->SetVertexBuffer(0, vertexBuffer.get());
				graphicsContext->SetIndexBuffer(indexBuffer.get());

				currentModel = modelComp.ModelHandle;
			}

			// 5. Preparar la memoria dinámica O(1) para la matriz de ESTE objeto
			// (Asumiendo que el Root Parameter 0 es un ConstantBufferView)
			Math::Matrix worldMatrix = transform.WorldTransform;
        
			DynamicAllocation alloc = graphicsContext->AllocateDynamicSpace(sizeof(Math::Matrix));
			memcpy(alloc.CPUAddress, &worldMatrix, sizeof(Math::Matrix));

			// 6. Enlazar datos volátiles a la Root Signature
			graphicsContext->SetConstantBuffer(0, alloc);               
			graphicsContext->SetPushConstant(1, modelComp.RuntimeMaterialIndex, 0);

			graphicsContext->DrawIndexedInstanced(mesh->GetIndexCount(),
			                                      1,
			                                      mesh->GetBaseIndex(),
			                                      mesh->GetBaseVertex(),
			                                      0);

		}
	}

	DirectX::BoundingOrientedBox SceneRenderer::CreateOBBFromOrthographicMatrix(const Math::Matrix& viewProj)
	{
		// Esquinas exactas del espacio NDC de DirectX 12
		Math::Vector3 ndcCorners[8] = {
			Math::Vector3(-1.0f,  1.0f, 0.0f), Math::Vector3( 1.0f,  1.0f, 0.0f),
			Math::Vector3( 1.0f, -1.0f, 0.0f), Math::Vector3(-1.0f, -1.0f, 0.0f),
			Math::Vector3(-1.0f,  1.0f, 1.0f), Math::Vector3( 1.0f,  1.0f, 1.0f),
			Math::Vector3( 1.0f, -1.0f, 1.0f), Math::Vector3(-1.0f, -1.0f, 1.0f)
		};

		Math::Matrix invViewProj = viewProj.Invert();
		Math::Vector3 worldCorners[8];

		// Llevamos las esquinas del cubo hacia el espacio del mundo real
		for (int i = 0; i < 8; ++i) {
			worldCorners[i] = Math::Vector3::Transform(ndcCorners[i], invViewProj);
		}

		// Dejamos que DirectXMath calcule el Centro, la Rotación y las Extensiones de esa caja
		DirectX::BoundingOrientedBox cascadeOBB;
		DirectX::BoundingOrientedBox::CreateFromPoints(cascadeOBB, 8, (const DirectX::XMFLOAT3*)worldCorners, sizeof(Math::Vector3));
    
		return cascadeOBB;
	}

	void SceneRenderer::PrepareCullingChunks(uint32_t numChunks, uint32_t chunkSize)
	{
		/*
		if (m_cullingChunks.size() < numChunks) {
			m_cullingChunks.resize(numChunks);
		}

		for (uint32_t i = 0; i < numChunks; ++i) {
			// Limpiamos resultados del frame anterior sin liberar la capacidad de RAM subyacente
			m_cullingChunks[i].VisibleEntities.clear();
			m_cullingChunks[i].VisibleEntities.reserve(chunkSize);
            
			for (uint32_t c = 0; c < NUM_CASCADES; ++c) {
				m_cullingChunks[i].ShadowCascades[c].clear();
				// Una reserva generosa, ya que cada cascada verá una fracción de la escena
				m_cullingChunks[i].ShadowCascades[c].reserve(chunkSize);
			}
		}
		*/
	}

	void SceneRenderer::ConsolidateFinalLists(uint32_t numChunks)
	{
		/*
		m_finalVisibleEntities.clear();
		for (uint32_t c = 0; c < NUM_CASCADES; ++c) {
			m_finalShadowEntities[c].clear();
		}

		// 1. Contar totales para hacer una sola alocación exacta de memoria maestra
		size_t totalVisible = 0;
		size_t totalShadows[NUM_CASCADES] = { 0 };

		for (uint32_t i = 0; i < numChunks; ++i) {
			totalVisible += m_cullingChunks[i].VisibleEntities.size();
			for (uint32_t c = 0; c < NUM_CASCADES; ++c) {
				totalShadows[c] += m_cullingChunks[i].ShadowCascades[c].size();
			}
		}

		m_finalVisibleEntities.reserve(totalVisible);
		for (uint32_t c = 0; c < NUM_CASCADES; ++c) {
			m_finalShadowEntities[c].reserve(totalShadows[c]);
		}

		// 2. Fusión masiva ultra rápida (Inserción O(n) contigua)
		for (uint32_t i = 0; i < numChunks; ++i) {
			m_finalVisibleEntities.insert(
				m_finalVisibleEntities.end(),
				m_cullingChunks[i].VisibleEntities.begin(),
				m_cullingChunks[i].VisibleEntities.end()
			);

			for (uint32_t c = 0; c < NUM_CASCADES; ++c) {
				m_finalShadowEntities[c].insert(
					m_finalShadowEntities[c].end(),
					m_cullingChunks[i].ShadowCascades[c].begin(),
					m_cullingChunks[i].ShadowCascades[c].end()
				);
			}
		}*/
	}
}
