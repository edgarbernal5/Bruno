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
#include "Bruno/Core/Memory.h"
#include "Bruno/Core/ScopedCpuTimer.h"
#include "Bruno/Platform/DirectX/DynamicAllocation.h"
#include "Bruno/Platform/DirectX/Profiler.h"
#include "Bruno/Platform/DirectX/VertexTypes.h"
#include "Bruno/Renderer/Camera.h"
#include "Bruno/Scene/Systems/FrustumCulling.h"
#include "Deferred/GBuffer.h"


namespace Bruno
{
	SceneRenderer::SceneRenderer(std::shared_ptr<Scene> scene, std::shared_ptr<FrustumCulling> frustumCulling, AbstractAssetManager* assetManager) :
		m_scene(scene),
		m_frustumCulling(frustumCulling),
		m_assetManager(assetManager)
	{
		auto& device = Graphics::GetDevice();
		
		m_globalSrvHeap = &device->GetSRVDescriptorAllocator();
		
		InitializeGBuffer(device);
		InitializeForwardRootSignature(device);
		InitializeGBufferRootSignature(device);
		InitializeDeferredRootSignature(device);
		
		InitializeForwardPSO(device);
		InitializeDeferredPSOs(device);
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
	
	void SceneRenderer::RenderForward(GraphicsContext* graphicsContext, Camera& camera, uint32_t frameIndex)
	{
		auto& device = Graphics::GetDevice();
		
		Profiler::Get().Stats.ResetCounters();
		ID3D12GraphicsCommandList* cmdList = graphicsContext->GetNative();
		
		ScopedCpuTimer totalCpuTimer(&Profiler::Get().Stats.CpuTotalRenderTimeMs);
		{
			ScopedCpuTimer cullingTimer(&Profiler::Get().Stats.CpuCullingTimeMs);
			m_frustumCulling->Update();
			Profiler::Get().Stats.TotalEntities = m_frustumCulling->GetTotalEntities();
			Profiler::Get().Stats.RenderedEntities = m_frustumCulling->GetTotalVisibleEntities();
		}
		auto& visibleEntities = m_frustumCulling->GetVisibleEntities();
		
		VertexBuffer* currentVB = nullptr;
		GraphicsPipelineState* currentPSO = nullptr;
		
		// 2. --- RENDERIZADO GPU ---
		Profiler::Get().StartGpuTimer(cmdList);

		m_materialManager->UpdateGPUBuffer(*graphicsContext);
		
		graphicsContext->SetPipelineState(m_forwardPSO.get());
		graphicsContext->SetRootSignature(m_forwardRootSig.get());
		
		ForwardLightingBuffer lightData = {};
		lightData.CameraPosition = camera.GetPosition();

		// 1. Luz Ambiental Fuerte para forzar visibilidad[cite: 1]
		lightData.GlobalAmbientColor = Math::Vector3(0.05f, 0.05f, 0.05f); 

		auto entitiesLightsGroup = m_scene->GetAllEntitiesWith<TransformComponent, DirectionalLightComponent>();
		for (auto lightEntity : entitiesLightsGroup)
		{
			auto [transformComponent, directionalLight] = entitiesLightsGroup.get<TransformComponent, DirectionalLightComponent>(lightEntity);
			
			lightData.Sun.Direction = directionalLight.Direction;
			lightData.Sun.Intensity = directionalLight.Intensity;
			lightData.Sun.Color = directionalLight.Color;
			break;
		}
		lightData.ActiveLightCount = 0;
		
		/*
		// =========================================================
		// LUZ 0: Key Light (Luz Principal)
		// Posicionada arriba, a la derecha y al frente. Tono cálido.
		// =========================================================
		lightData.Lights[0].Position = Math::Vector3(5.0f, 5.0f, -5.0f);
		lightData.Lights[0].Color = Math::Vector3(1.0f, 0.9f, 0.8f); // Ligeramente naranja/cálido
		lightData.Lights[0].Radius = 50.0f;
		lightData.Lights[0].Intensity = 500.0f; // Suficientemente alta para vencer el 1/d^2

		// =========================================================
		// LUZ 1: Fill Light (Luz de Relleno)
		// Posicionada en el lado opuesto, más baja. Tono frío.
		// Evita que las sombras sean 100% negras y añade contraste.
		// =========================================================
		lightData.Lights[1].Position = Math::Vector3(-8.0f, 2.0f, -3.0f);
		lightData.Lights[1].Color = Math::Vector3(0.6f, 0.8f, 1.0f); // Ligeramente azul/celeste
		lightData.Lights[1].Radius = 50.0f;
		lightData.Lights[1].Intensity = 200.0f; // Menos intensa que la luz principal

		// =========================================================
		// LUZ 2: Rim / Back Light (Luz de Contraluz)
		// Posicionada detrás del modelo. Blanca y brillante.
		// FUNDAMENTAL en PBR: Resalta los bordes (Fresnel) de los materiales.
		// =========================================================
		lightData.Lights[2].Position = Math::Vector3(0.0f, 6.0f, 8.0f);
		lightData.Lights[2].Color = Math::Vector3(1.0f, 1.0f, 1.0f);
		lightData.Lights[2].Radius = 50.0f;
		lightData.Lights[2].Intensity = 800.0f; // Muy alta para que los bordes destaquen
		*/
		m_forwardLightsCB.Update(*graphicsContext, lightData);
		
		// Setear Luces en el Índice 2 (b2)
		graphicsContext->SetConstantBuffer(2, m_forwardLightsCB);
		graphicsContext->SetDescriptorHeaps({ m_globalSrvHeap });
		
		// Vincular el StructuredBuffer de Materiales a 't0' (Índice 3) usando su Allocation específica
		graphicsContext->SetDescriptorTable(3, m_materialManager->GetSRVAllocation());
		graphicsContext->SetDescriptorTable(4, device->GetSRVDescriptorAllocator());
		
		for (Entity entity : visibleEntities)
		{
			const auto& modelComponent = entity.GetComponent<ModelComponent>();
			const auto& transformComponent = entity.GetComponent<TransformComponent>();
			
			if (modelComponent.RuntimeMaterialIndex == 0xFFFFFFFF)
			{
				continue;
			}
			
			auto model = m_assetManager->GetAsset<Model>(modelComponent.ModelHandle);
			uint32_t meshIndex = modelComponent.MeshIndex;
			auto& meshes = model->GetMeshes();
			auto& mesh = meshes[meshIndex];
						
			auto& indexBuffer = model->GetIndexBuffer();
			auto& vertexBuffer = model->GetVertexBuffer();
			if (currentVB != vertexBuffer.get())
			{
				graphicsContext->SetVertexBuffer(0, vertexBuffer.get());
				graphicsContext->SetIndexBuffer(indexBuffer.get());
				currentVB = vertexBuffer.get();
			}
			// Preparar las transformaciones (b0) - ¡Ahora sin la cámara!
			const Math::Matrix& world = transformComponent.WorldTransform;
			//Math::Matrix wvp = (world * camera.GetViewProjection()).Transpose();
			
			SceneObjectBuffer objConstants;
			objConstants.World = world.Transpose();
			objConstants.ViewProjection = camera.GetViewProjection().Transpose();
			
			// Alocar dinámicamente y bindear al Root Parameter 0 (b0)
			ConstantBuffer<SceneObjectBuffer> transformCB;
			transformCB.Update(*graphicsContext, objConstants);
			graphicsContext->SetConstantBuffer(0, transformCB);

			// 7. Bindear el ID Bindless del material (b1)
			graphicsContext->SetPushConstant(1, modelComponent.RuntimeMaterialIndex, 0);
			
			graphicsContext->SetPrimitiveTopology(PrimitiveTopology::TriangleList);
			graphicsContext->DrawIndexedInstanced(mesh->GetIndexCount(),
			                                      1,
			                                      mesh->GetBaseIndex(),
			                                      mesh->GetBaseVertex(),
			                                      0);
			
			Profiler::Get().Stats.DrawCalls++;
			Profiler::Get().Stats.TriangleCount += (mesh->GetIndexCount() / 3);
			
		}
		
		Profiler::Get().StopGpuTimer(cmdList);
		
		// Le ordenamos a la GPU copiar los tiempos al buffer leíble
		Profiler::Get().ResolveGpuTimestamps(cmdList);
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

	void SceneRenderer::InitializeGBuffer(GraphicsDevice* device)
	{
		m_gBuffer = std::make_shared<GBuffer>();
		m_gBuffer->Initialize(*device, device->GetSRVDescriptorAllocator(), device->GetRTVDescriptorAllocator(), 100, 100);
	}

	void SceneRenderer::InitializeForwardRootSignature(GraphicsDevice* device)
	{
		auto prototypeSig = std::make_shared<RootSignature>(*device);

		// b0: Transformaciones. ¡Visible SOLO para el Vertex Shader recuperado!
		prototypeSig->AddConstantBufferView(0, 0, ShaderVisibility::Vertex);

		// b1: ID del Material Bindless.
		prototypeSig->AddConstants(1, 1, 0, ShaderVisibility::Pixel);

		// b2: Arreglo de Luces Fijas Y Posición de la Cámara (Datos de Iluminación).
		prototypeSig->AddConstantBufferView(2, 0, ShaderVisibility::Pixel);

		// t0: StructuredBuffer de Materiales (1 solo descriptor)
		prototypeSig->AddDescriptorTableSRV(1, 0, 0, ShaderVisibility::Pixel);

		// t1: Arreglo infinito de Texturas Bindless (-1 / UINT_MAX)
		prototypeSig->AddDescriptorTableSRV(UINT_MAX, 1, 0, ShaderVisibility::Pixel);

		// s0: Sampler principal (Wrap, Anisotrópico, etc.)
		prototypeSig->AddStaticSampler(
			0, 0,
			TextureFilter::Anisotropic,
			TextureAddressMode::Wrap,
			ShaderVisibility::Pixel
		);

		m_forwardRootSig = RootSignatureLibrary::GetOrCreate(prototypeSig);
	}

	void SceneRenderer::InitializeForwardPSO(GraphicsDevice* device)
	{
		GraphicsPipelineStateDesc psoDesc = {};
		// Definir el Input Layout (DEBE COINCIDIR CON ModelVertex Y CON EL HLSL)
		psoDesc.RootSignature = m_forwardRootSig.get();
		psoDesc.InputLayout = VertexPositionNormalTexture::GetLayout();
		
		psoDesc.VertexShaderDesc = { L"Shaders/ForwardOpaque.hlsl", L"VSMain", L"vs_6_0" };
		psoDesc.PixelShaderDesc = { L"Shaders/ForwardOpaque.hlsl", L"PSMain", L"ps_6_0" };
		
		psoDesc.RasterizerDesc.CullMode = CullMode::Back;
		psoDesc.RasterizerDesc.FillMode = FillMode::Solid;
		psoDesc.RasterizerDesc.FrontCounterClockwise = true;
        
		psoDesc.Topology = PrimitiveTopology::TriangleList;
    
		// Formatos de Salida (DEBEN coincidir con tu SwapChain y DepthBuffer)
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = TextureFormat::R8G8B8A8_Unorm;
		psoDesc.DSVFormat = TextureFormat::D24_Unorm_S8_Uint;
		
		m_forwardPSO = PSOCache::GetOrCreate(device, psoDesc);
	}

	void SceneRenderer::InitializeGBufferRootSignature(GraphicsDevice* device)
	{
		auto prototypeSig = std::make_shared<RootSignature>(*device);
		
		// =========================================================
		// 2. CONSTANT BUFFERS (Transformaciones y Materiales)
		// =========================================================
		// b0: TransformBuffer (g_World, g_ViewProjection)
		// Exclusivo para el Vertex Shader para evitar procesamientos innecesarios en el Pixel Shader
		prototypeSig->AddConstantBufferView(0, 0, ShaderVisibility::Vertex);

		// b1: MaterialConstant (g_MaterialIndex)
		// Un único valor Root Constant de 32-bits que indica qué material del buffer usar.
		prototypeSig->AddConstants(1, 1, 0, ShaderVisibility::Pixel);

		// =========================================================
		// 3. TABLAS DE DESCRIPTORES (Ecosistema Bindless)
		// =========================================================
		// t0: StructuredBuffer<MaterialData> (g_MaterialBuffer)
		// Tabla con 1 solo descriptor que apunta al búfer estructurado de materiales.
		prototypeSig->AddDescriptorTableSRV(1, 0, 0, ShaderVisibility::Pixel);

		// t1: Texture2D g_Textures[] (Arreglo Infinito Bindless)
		// Usamos UINT_MAX (-1) para instruir a DirectX 12 que el tamaño de este arreglo es ilimitado.
		prototypeSig->AddDescriptorTableSRV(UINT_MAX, 1, 0, ShaderVisibility::Pixel);

		// =========================================================
		// 4. SAMPLERS ESTÁTICOS
		// =========================================================
		// s0: Sampler principal (g_Sampler)
		// Configurado como Anisotrópico y Wrap para máxima calidad visual en los modelos 3D
		prototypeSig->AddStaticSampler(
			0, 
			0, 
			TextureFilter::Anisotropic, 
			TextureAddressMode::Wrap, 
			ShaderVisibility::Pixel
		);
		
		m_gbufferRootSig = RootSignatureLibrary::GetOrCreate(prototypeSig);
	}

	void SceneRenderer::InitializeDeferredRootSignature(GraphicsDevice* device)
	{
		auto prototypeSig = std::make_shared<RootSignature>(*device);
		
		// =========================================================
		// 1. CONSTANT BUFFERS
		// =========================================================
		// b0: Datos de la Luz, Cámara y Matrices ortográficas de sombras
		prototypeSig->AddConstantBufferView(0, 0, ShaderVisibility::Pixel);
		// =========================================================
		// 2. TABLAS DE DESCRIPTORES (TEXTURAS)
		// =========================================================
		// t0, t1, t2: Texturas del G-Buffer (Albedo, Normales, Posición)
		// Pedimos 3 descriptores contiguos comenzando en el registro 0
		prototypeSig->AddDescriptorTableSRV(3, 0, 0, ShaderVisibility::Pixel);

		// t3: Texture2DArray de Sombras en Cascada (CSM)
		prototypeSig->AddDescriptorTableSRV(1, 3, 0, ShaderVisibility::Pixel);

		// =========================================================
		// 3. SAMPLERS ESTÁTICOS
		// =========================================================
		// s0: Shadow Comparison Sampler (Muestreo PCF por Hardware)
		prototypeSig->AddStaticSampler(
			0, 0, 
			TextureFilter::Comparison_MinMag_Linear_MipPoint, 
			TextureAddressMode::Border, // Borde blanco fuera del mapa para que no haya sombras
			ShaderVisibility::Pixel
		);

		// s1: Linear Sampler (Para leer el G-Buffer con suavizado)
		prototypeSig->AddStaticSampler(
			1, 0, 
			TextureFilter::Linear, 
			TextureAddressMode::Clamp, 
			ShaderVisibility::Pixel
		);
		
		m_deferredLightingRootSig = RootSignatureLibrary::GetOrCreate(prototypeSig);
	}

	void SceneRenderer::InitializeDeferredPSOs(GraphicsDevice* device)
	{
		GraphicsPipelineStateDesc gbufferDesc = {};
		gbufferDesc.RootSignature = m_gbufferRootSig.get();
		
		gbufferDesc.VertexShaderDesc = { L"Shaders/GBufferPass.hlsl", L"VSMain", L"vs_6_0" };
		gbufferDesc.PixelShaderDesc  = { L"Shaders/GBufferPass.hlsl", L"PSMain", L"ps_6_0" };
		
		gbufferDesc.InputLayout = VertexPositionNormalTexture::GetLayout();
		
		gbufferDesc.Topology = PrimitiveTopology::TriangleList;
		gbufferDesc.DepthState.Mode = DepthMode::ReadWrite;
    
		// Múltiples Render Targets (MRT)
		gbufferDesc.NumRenderTargets = 3;
		gbufferDesc.RTVFormats[0] = TextureFormat::R8G8B8A8_Unorm;       // Albedo + Metal
		gbufferDesc.RTVFormats[1] = TextureFormat::R16G16B16A16_Float;   // Normal + Rough
		gbufferDesc.RTVFormats[2] = TextureFormat::R16G16B16A16_Float;   // Position
		gbufferDesc.DSVFormat = TextureFormat::D32_Float;
    
		m_gbufferPSO = PSOCache::GetOrCreate(device, gbufferDesc);
		
		// PSO de Iluminación Diferida (Full-Screen Triangle)
		GraphicsPipelineStateDesc deferredDesc = {};
		deferredDesc.RootSignature = m_deferredLightingRootSig.get();
		deferredDesc.VertexShaderDesc = { L"Shaders/DeferredLighting.hlsl", L"VSMain", L"vs_6_0" }; // Genera el triángulo con SV_VertexID
		deferredDesc.PixelShaderDesc  = { L"Shaders/DeferredLighting.hlsl", L"PSMain", L"ps_6_0" };
    
		// Escribimos a la pantalla, sin Depth Buffer
		deferredDesc.NumRenderTargets = 1;
		deferredDesc.RTVFormats[0] = TextureFormat::B8G8R8A8_Unorm; 
		deferredDesc.DSVFormat = TextureFormat::Unknown;
		deferredDesc.DepthState.Mode = DepthMode::None;
    
		m_deferredLightingPSO = PSOCache::GetOrCreate(device, deferredDesc);
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
			gpuData.AlbedoTextureIndex = 0xFFFFFFFF; // El shader debe ignorarlo
		}

		// ... (Haces lo mismo para NormalTextureIndex y otros mapas) ...

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
}
