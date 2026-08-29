#include "brpch.h"
#include "SceneRenderer.h"

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
		auto& device = Bruno::Graphics::GetDevice();
		
		InitializeGbuffer(device);
		InitializeOpaqueRootSignature(device);
		InitializeGbufferRootSignature(device);
		InitializeDeferredRootSignature(device);
		
		InitializeOpaquePSO(device);
		InitializeDeferredPSOs(device);
		InitializeShadowPipeline(device);
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
	
	void SceneRenderer::RenderScene(GraphicsContext* graphicsContext, Camera& camera, uint32_t frameIndex)
	{
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

	void SceneRenderer::RenderDeferred(GraphicsContext* context, Camera& camera, uint32_t frameIndex)
	{
		// ==========================================
		// FASE 2: G-BUFFER PASS (Geometría Opaca)
		// ==========================================
		context->TransitionResource(m_gBuffer->GetAlbedoMetalness(), ResourceState::PixelShaderResource, ResourceState::RenderTarget);
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
		context->DrawInstanced(3, 1, 0, 0);
	}

	void SceneRenderer::InitializeGbuffer(GraphicsDevice* device)
	{
		m_gBuffer = std::make_shared<GBuffer>();
		m_gBuffer->Initialize(*device, device->GetSRVDescriptorAllocator(), device->GetRTVDescriptorAllocator(),100,100);
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
		
		psoDesc.RasterizerDesc.CullMode = CullMode::Back;
		psoDesc.RasterizerDesc.FillMode = FillMode::Solid;
		psoDesc.RasterizerDesc.FrontCounterClockwise = true;
        
		psoDesc.Topology = PrimitiveTopology::TriangleList;
    
		// Formatos de Salida (DEBEN coincidir con tu SwapChain y DepthBuffer)
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = TextureFormat::R8G8B8A8_Unorm;
		psoDesc.DSVFormat = TextureFormat::D24_Unorm_S8_Uint;
		
		m_opaquePSO = PSOCache::GetOrCreate(device, psoDesc);
	}

	void SceneRenderer::InitializeGbufferRootSignature(GraphicsDevice* device)
	{
		auto prototypeSig = std::make_shared<RootSignature>(*device);
		
		// =========================================================
		// 2. CONSTANT BUFFERS (Transformaciones y Materiales)
		// =========================================================
		// b0: TransformBuffer (g_World, g_ViewProjection)
		// Exclusivo para el Vertex Shader para evitar procesamientos innecesarios en el Pixel Shader[cite: 1, 7].
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
		// Configurado como Anisotrópico y Wrap para máxima calidad visual en los modelos 3D[cite: 3, 7].
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

}
