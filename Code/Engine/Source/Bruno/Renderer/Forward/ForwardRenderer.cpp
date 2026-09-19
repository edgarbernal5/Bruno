#include "brpch.h"
#include "ForwardRenderer.h"

#include "Bruno/Platform/DirectX/GraphicsPipelineState.h"
#include "Bruno/Platform/DirectX/RootSignature.h"
#include "Bruno/Platform/DirectX/VertexTypes.h"
#include "Bruno/Renderer/PSOCache.h"
#include "Bruno/Renderer/RootSignatureLibrary.h"

namespace Bruno
{
    ForwardRenderer::ForwardRenderer(GraphicsDevice* device, std::shared_ptr<Scene> scene) :
        m_scene(scene)
    {
    }

    void ForwardRenderer::Render(GraphicsContext* graphicsContext, Camera& camera, uint32_t frameIndex)
    {
    	/*
		auto& device = Graphics::GetDevice();
		
		Profiler::Get().Stats.ResetCounters();
		ID3D12GraphicsCommandList* cmdList = graphicsContext->GetNative();
		
		ScopedCpuTimer totalCpuTimer(&Profiler::Get().Stats.CpuTotalRenderTimeMs);
		{
			ScopedCpuTimer cullingTimer(&Profiler::Get().Stats.CpuCullingTimeMs);
			//m_frustumCulling->Update();
			ExecuteMassiveCulling(camera);
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

		// 1. Luz Ambiental Fuerte para forzar visibilidad
		lightData.GlobalAmbientColor = Math::Vector3(0.05f, 0.05f, 0.05f); 

		auto entitiesLightsGroup = m_scene->GetAllEntitiesWith<TransformComponent, DirectionalLightComponent>();
		for (auto lightEntity : entitiesLightsGroup)
		{
			auto [transformComponent, directionalLight] = entitiesLightsGroup.get<TransformComponent, DirectionalLightComponent>(lightEntity);
			
			auto forward = transformComponent.WorldTransform.Forward();
			forward.Normalize();
			lightData.Sun.Direction = forward;
			lightData.Sun.Intensity = directionalLight.Intensity;
			lightData.Sun.Color = directionalLight.Color;
			
			m_directionalLightDir = forward;
			break;
		}
		lightData.ActivePointLightCount = 0;
		lightData.ActiveSpotLightCount = 0;
		
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
			
			SceneObjectBuffer objConstants;
			objConstants.World = world.Transpose();
			objConstants.ViewProjection = camera.GetViewProjection().Transpose();
			
			// Alocar dinámicamente y bindear al Root Parameter 0 (b0)
			ConstantBuffer<SceneObjectBuffer> transformCB;
			transformCB.Update(*graphicsContext, objConstants);
			graphicsContext->SetConstantBuffer(0, transformCB);

			// Bindear el ID Bindless del material (b1)
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
    	*/
	}

    void ForwardRenderer::InitializeForwardRootSignature(GraphicsDevice* device)
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

    void ForwardRenderer::InitializeForwardPSO(GraphicsDevice* device)
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
}
