#include "brpch.h"
#include "SceneRendererDeferred.h"

#include "GBuffer.h"
#include "Bruno/Content/AssetManager.h"
#include "Bruno/Platform/DirectX/ColorBuffer.h"
#include "Bruno/Platform/DirectX/CommandQueueManager.h"
#include "Bruno/Platform/DirectX/DepthBuffer.h"
#include "Bruno/Platform/DirectX/GraphicsDevice.h"
#include "Bruno/Platform/DirectX/GraphicsPipelineState.h"
#include "Bruno/Platform/DirectX/IndexBuffer.h"
#include "Bruno/Platform/DirectX/Profiler.h"
#include "Bruno/Platform/DirectX/RootSignature.h"
#include "Bruno/Platform/DirectX/VertexBuffer.h"
#include "Bruno/Platform/DirectX/VertexTypes.h"
#include "Bruno/Renderer/MaterialManager.h"
#include "Bruno/Renderer/Model.h"
#include "Bruno/Renderer/PSOCache.h"
#include "Bruno/Renderer/RootSignatureLibrary.h"
#include "Bruno/Renderer/Shadows/ShadowMapArray.h"
#include "Bruno/Renderer/Shadows/ShadowSystem.h"
#include "Bruno/Scene/Scene.h"
#include "Bruno/Scene/Systems/CullingSystem.h"

namespace Bruno
{
    SceneRendererDeferred::SceneRendererDeferred(GraphicsDevice* device, std::shared_ptr<Scene> scene, std::shared_ptr<MaterialManager> materialManager, std::shared_ptr<ShadowMapArray> shadowMapArray, AbstractAssetManager* assetManager, ShadowProperties* shadowProperties, uint32_t initialWidth, uint32_t initialHeight) :
        m_device(device),
        m_scene(scene),
        m_materialManager(materialManager),
        m_shadowMapArray(shadowMapArray),
        m_assetManager(assetManager),
        m_shadowProperties(shadowProperties),
        m_viewportWidth(initialWidth),
        m_viewportHeight(initialHeight)
    {
        m_globalSrvHeap = &device->GetSRVDescriptorAllocator();

        InitializeGBuffer(device, initialWidth, initialHeight);
        InitializeGBufferRootSignature(device);
        InitializeDeferredRootSignature(device);
        InitializeLocalLightsRootSignature(device);
        InitializeDeferredPSOs(device);
        InitializeProxyMeshes(device);

        m_outsidePointLights.reserve(512);
        m_insidePointLights.reserve(64);
        m_outsideSpotLights.reserve(256);
        m_insideSpotLights.reserve(64);
    }

    void SceneRendererDeferred::Render(GraphicsContext* graphicsContext, Camera& camera, uint32_t frameIndex, ColorBuffer* targetRenderTarget, const std::vector<CascadeData>& cascades, const FrameCullingResults& cullingData)
    {
        if (m_lastShadowFilterQuality != m_shadowProperties->m_filterQuality)
        {
            InitializeDeferredRootSignature(m_device);
            InitializeDeferredPSOs(m_device);
        }

        m_materialManager->UpdateGPUBuffer(*graphicsContext);
        graphicsContext->SetDescriptorHeaps({ m_globalSrvHeap });

        // =========================================================================
        // FASE 1: G-BUFFER PASS (Geometría Opaca -> MRT)
        // =========================================================================
        RenderGBufferPass(graphicsContext, camera, cullingData);

        // =========================================================================
        // FASE 2: DEFERRED DIRECTIONAL LIGHTING + CSM (Full-Screen Triangle)
        // =========================================================================
        RenderDirectionalLightPass(graphicsContext, camera, targetRenderTarget, cascades);

        // =========================================================================
        // FASE 3: LOCAL LIGHTS PASS (Point & Spot Lights Instanciadas)
        // =========================================================================
        RenderLocalLightsPass(graphicsContext, camera, targetRenderTarget);
    }

    void SceneRendererDeferred::Resize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0 || (width == m_viewportWidth && height == m_viewportHeight))
        {
            return;
        }

        m_viewportWidth = width;
        m_viewportHeight = height;
        m_gBuffer->Resize(*m_device, m_device->GetSRVDescriptorAllocator(), m_device->GetRTVDescriptorAllocator(), width, height);
    }
    
    void SceneRendererDeferred::InitializeGBuffer(GraphicsDevice* device, uint32_t width, uint32_t height)
    {
        m_gBuffer = std::make_shared<GBuffer>();
        m_gBuffer->Initialize(*device, *m_globalSrvHeap, device->GetRTVDescriptorAllocator(), width, height);
    }

    void SceneRendererDeferred::InitializeGBufferRootSignature(GraphicsDevice* device)
    {
        auto prototypeSig = std::make_shared<RootSignature>(*device);
		
        // b0: TransformBuffer (World, ViewProjection) -> Solo VS
        prototypeSig->AddConstantBufferView(0, 0, ShaderVisibility::Vertex);

        // b1: MaterialIndex Bindless -> Solo PS
        prototypeSig->AddConstants(1, 1, 0, ShaderVisibility::Pixel);

        // t0: StructuredBuffer<MaterialData>
        prototypeSig->AddDescriptorTableSRV(1, 0, 0, ShaderVisibility::Pixel);

        // t1: Texture2D g_Textures[] (Arreglo Infinito Bindless)
        prototypeSig->AddDescriptorTableSRV(UINT_MAX, 1, 0, ShaderVisibility::Pixel);

        // s0: Sampler Anisotrópico Wrap
        prototypeSig->AddStaticSampler(0, 0, TextureFilter::Anisotropic, TextureAddressMode::Wrap, ShaderVisibility::Pixel);

        m_gbufferRootSig = RootSignatureLibrary::GetOrCreate(prototypeSig);
    }

    void SceneRendererDeferred::InitializeDeferredRootSignature(GraphicsDevice* device)
    {
        auto prototypeSig = std::make_shared<RootSignature>(*device);

        // 0: b0 -> DeferredLightingBuffer
        prototypeSig->AddConstantBufferView(0, 0, ShaderVisibility::Pixel);

        // 1, 2, 3: t0, t1, t2 -> Cada textura del G-Buffer en su propia tabla de 1 SRV
        prototypeSig->AddDescriptorTableSRV(1, 0, 0, ShaderVisibility::Pixel); // t0: AlbedoMetal
        prototypeSig->AddDescriptorTableSRV(1, 1, 0, ShaderVisibility::Pixel); // t1: NormalRough
        prototypeSig->AddDescriptorTableSRV(1, 2, 0, ShaderVisibility::Pixel); // t2: Position

        // 4: t3 -> Texture2DArray de Cascaded Shadow Maps
        prototypeSig->AddDescriptorTableSRV(1, 3, 0, ShaderVisibility::Pixel);

        // Samplers estáticos (sin cambios)
        if (m_shadowProperties->m_filterQuality == ShadowFilterQuality::None)
        {
            prototypeSig->AddStaticSampler(0, 0, TextureFilter::Comparison_Point, TextureAddressMode::Clamp, ShaderVisibility::Pixel, ComparisonFunc::LessEqual);
        }
        else
        {
            prototypeSig->AddStaticSampler(0, 0, TextureFilter::Comparison_MinMag_Linear_MipPoint, TextureAddressMode::Clamp, ShaderVisibility::Pixel, ComparisonFunc::LessEqual);
        }
        prototypeSig->AddStaticSampler(1, 0, TextureFilter::Linear, TextureAddressMode::Clamp, ShaderVisibility::Pixel);

        m_lastShadowFilterQuality = m_shadowProperties->m_filterQuality;
        m_deferredLightingRootSig = RootSignatureLibrary::GetOrCreate(prototypeSig);
    }

    void SceneRendererDeferred::InitializeLocalLightsRootSignature(GraphicsDevice* device)
    {
        auto prototypeSig = std::make_shared<RootSignature>(*device);

        // 0: b0 -> LocalLightsCameraBuffer
        prototypeSig->AddConstantBufferView(0, 0, ShaderVisibility::All);

        // 1: t0 -> StructuredBuffer<LightInstanceData>
        prototypeSig->AddShaderResourceView(0, 0, ShaderVisibility::All);

        // 2, 3, 4: t1, t2, t3 -> G-Buffer individual
        prototypeSig->AddDescriptorTableSRV(1, 1, 0, ShaderVisibility::Pixel); // t1: AlbedoMetal
        prototypeSig->AddDescriptorTableSRV(1, 2, 0, ShaderVisibility::Pixel); // t2: NormalRough
        prototypeSig->AddDescriptorTableSRV(1, 3, 0, ShaderVisibility::Pixel); // t3: Position

        // s0: Linear Sampler
        prototypeSig->AddStaticSampler(0, 0, TextureFilter::Linear, TextureAddressMode::Clamp, ShaderVisibility::Pixel);

        m_localLightsRootSig = RootSignatureLibrary::GetOrCreate(prototypeSig);
    }

    void SceneRendererDeferred::InitializeDeferredPSOs(GraphicsDevice* device)
    {
        GraphicsPipelineStateDesc gbufferDesc = {};
        gbufferDesc.RootSignature = m_gbufferRootSig.get();
		
        gbufferDesc.VertexShaderDesc = { L"Shaders/GBufferPass.hlsl", L"VSMain", L"vs_6_0" };
        gbufferDesc.PixelShaderDesc  = { L"Shaders/GBufferPass.hlsl", L"PSMain", L"ps_6_0" };
		
        gbufferDesc.InputLayout = VertexPositionNormalTexture::GetLayout();
		
        gbufferDesc.RasterizerDesc.CullMode = CullMode::Back;
        gbufferDesc.RasterizerDesc.FillMode = FillMode::Solid;
        gbufferDesc.RasterizerDesc.FrontCounterClockwise = true; // Consistente con ForwardRenderer

        gbufferDesc.Topology = PrimitiveTopology::TriangleList;
        gbufferDesc.DepthState.Mode = DepthMode::ReadWrite;
        gbufferDesc.DepthState.Func = ComparisonFunc::Less;

        gbufferDesc.NumRenderTargets = 3;
        gbufferDesc.RTVFormats[0] = TextureFormat::R8G8B8A8_Unorm;
        gbufferDesc.RTVFormats[1] = TextureFormat::R16G16B16A16_Float;
        gbufferDesc.RTVFormats[2] = TextureFormat::R16G16B16A16_Float;
        gbufferDesc.DSVFormat = TextureFormat::D32_Float;
    
        m_gbufferPSO = PSOCache::GetOrCreate(device, gbufferDesc);
		
        // PSO de Iluminación Diferida (Full-Screen Triangle)
        GraphicsPipelineStateDesc deferredDesc = {};
        deferredDesc.RootSignature = m_deferredLightingRootSig.get();
        deferredDesc.VertexShaderDesc = { L"Shaders/DeferredLighting.hlsl", L"VSMain", L"vs_6_0" }; // Genera el triángulo con SV_VertexID
        deferredDesc.PixelShaderDesc  = { L"Shaders/DeferredLighting.hlsl", L"PSMain", L"ps_6_0" };
    
        deferredDesc.RasterizerDesc.CullMode = CullMode::None;
        deferredDesc.RasterizerDesc.FillMode = FillMode::Solid;
        deferredDesc.Topology = PrimitiveTopology::TriangleList;

        deferredDesc.NumRenderTargets = 1;
        deferredDesc.RTVFormats[0] = TextureFormat::R8G8B8A8_Unorm; // Alineado con ForwardRenderer
        deferredDesc.DSVFormat = TextureFormat::Unknown;
        deferredDesc.DepthState.Mode = DepthMode::None;
    
        m_deferredLightingPSO = PSOCache::GetOrCreate(device, deferredDesc);
        
        // Local Lights PSOs (Additive Blending + Early-Z Rejection)
        GraphicsPipelineStateDesc localLightDesc = {};
        localLightDesc.RootSignature = m_localLightsRootSig.get();
        localLightDesc.VertexShaderDesc = { L"Shaders/LocalLights.hlsl", L"VSMain", L"vs_6_0" };
        localLightDesc.PixelShaderDesc  = { L"Shaders/LocalLights.hlsl", L"PSMain", L"ps_6_0" };
        localLightDesc.InputLayout = VertexPosition::GetLayout();

        localLightDesc.Topology = PrimitiveTopology::TriangleList;
        localLightDesc.BlendState.Mode = BlendMode::Additive;
        localLightDesc.DepthState.Mode = DepthMode::ReadOnly;

        localLightDesc.NumRenderTargets = 1;
        localLightDesc.RTVFormats[0] = TextureFormat::R8G8B8A8_Unorm;
        localLightDesc.DSVFormat = TextureFormat::D32_Float;

        // A) Cámara Afuera del Volumen
        localLightDesc.RasterizerDesc.CullMode = CullMode::Back;
        localLightDesc.RasterizerDesc.FrontCounterClockwise = true;
        localLightDesc.DepthState.Func = ComparisonFunc::LessEqual;
        m_localLightOutsidePSO = PSOCache::GetOrCreate(device, localLightDesc);

        // B) Cámara Adentro del Volumen
        localLightDesc.RasterizerDesc.CullMode = CullMode::Front;
        localLightDesc.RasterizerDesc.FrontCounterClockwise = true;
        localLightDesc.DepthState.Func = ComparisonFunc::GreaterEqual;
        m_localLightInsidePSO = PSOCache::GetOrCreate(device, localLightDesc);
    }

    void SceneRendererDeferred::InitializeProxyMeshes(GraphicsDevice* device)
    {
        auto& uploadContext = device->GetUploadContext();
        auto& commandManager = Graphics::GetCommandQueueManager();
        
        uploadContext.Reset();
        
        const uint16_t stacks = 16;
        const uint16_t slices = 16;
        
        // =========================================================================
        // 1. ESFERA UNITARIA (Radio 1.0) para Point Lights
        // =========================================================================
        std::vector<VertexPosition> sphereVerts;
        std::vector<uint16_t> sphereIndices;
        sphereVerts.reserve((stacks + 1) * (slices + 1));
        sphereIndices.reserve(stacks * slices * 6);

        for (uint16_t i = 0; i <= stacks; ++i)
        {
            float phi = Math::PI * static_cast<float>(i) / static_cast<float>(stacks);
            float y = std::cos(phi);
            float r = std::sin(phi);

            for (uint16_t j = 0; j <= slices; ++j)
            {
                float theta = Math::TWO_PI * static_cast<float>(j) / static_cast<float>(slices);
                float x = r * std::cos(theta);
                float z = r * std::sin(theta);

                sphereVerts.push_back({ Math::Vector3(x, y, z) });
            }
        }

        for (uint16_t i = 0; i < stacks; ++i)
        {
            for (uint16_t j = 0; j < slices; ++j)
            {
                uint16_t first  = i * (slices + 1) + j;
                uint16_t second = first + slices + 1;

                sphereIndices.push_back(first);
                sphereIndices.push_back(first + 1);
                sphereIndices.push_back(second);

                sphereIndices.push_back(second);
                sphereIndices.push_back(first + 1);
                sphereIndices.push_back(second + 1);
            }
        }

        /*
        auto& uploadContext = device->GetUploadContext();
        auto& commandManager = Graphics::GetCommandQueueManager();
        
        uploadContext.Reset();
        
        m_indexBuffer = std::make_shared<IndexBuffer>(*device, uploadContext, m_indices);
        m_vertexBuffer = std::make_shared<VertexBuffer>(*device, uploadContext, verticesPNT.data(), verticesPNT.size(), sizeof(VertexPositionNormalTexture));
        
        uint64_t uploadFence = commandManager->ExecuteAndReturnFence(uploadContext);

        commandManager->WaitForGpuFence(uploadFence);

        uploadContext.ClearGarbage();
         */
        
        m_unitSphereIndexCount = static_cast<uint32_t>(sphereIndices.size());
        m_unitSphereVB = std::make_unique<VertexBuffer>(
            *device,
            uploadContext,
            sphereVerts.data(),
            sphereVerts.size(),
            sizeof(VertexPosition),
            L"ProxySphere_VB"
        );
        m_unitSphereIB = std::make_unique<IndexBuffer>(
            *device,
            uploadContext,
            sphereIndices,
            L"ProxySphere_IB"
        );
        
        // =========================================================================
        // 2. CONO UNITARIO (Ápice en origen, altura 1 en -Z, radio base 1) para Spot Lights
        // =========================================================================
        std::vector<VertexPosition> coneVerts;
        std::vector<uint16_t> coneIndices;
        coneVerts.reserve(slices + 3);
        coneIndices.reserve(slices * 6);

        coneVerts.push_back({ Math::Vector3(0.0f, 0.0f,  0.0f) }); // Índice 0: Ápice
        coneVerts.push_back({ Math::Vector3(0.0f, 0.0f, -1.0f) }); // Índice 1: Centro base

        for (uint16_t j = 0; j <= slices; ++j)
        {
            float theta = Math::TWO_PI * static_cast<float>(j) / static_cast<float>(slices);
            coneVerts.push_back({ Math::Vector3(std::cos(theta), std::sin(theta), -1.0f) });
        }

        for (uint16_t j = 0; j < slices; ++j)
        {
            uint16_t curr = 2 + j;
            uint16_t next = 2 + j + 1;

            // Manto lateral
            coneIndices.push_back(0);
            coneIndices.push_back(next);
            coneIndices.push_back(curr);

            // Tapa base cerrada (hermética para Depth Culling inverso)
            coneIndices.push_back(1);
            coneIndices.push_back(curr);
            coneIndices.push_back(next);
        }

        m_unitConeVB = std::make_unique<VertexBuffer>(
            *device,
            uploadContext,
            coneVerts.data(),
            coneVerts.size(),
            sizeof(VertexPosition),
            L"ProxyCone_VB"
        );
        m_unitConeIB = std::make_unique<IndexBuffer>(
            *device,
            uploadContext,
            coneIndices,
            L"ProxyCone_IB"
        );
        
        m_unitConeIndexCount = static_cast<uint32_t>(coneIndices.size());
        uint64_t uploadFence = commandManager->ExecuteAndReturnFence(uploadContext);

        commandManager->WaitForGpuFence(uploadFence);

        uploadContext.ClearGarbage();
    }

    void SceneRendererDeferred::RenderGBufferPass(GraphicsContext* graphicsContext, Camera& camera, const FrameCullingResults& cullingData)
    {
        auto* albedoMetal = m_gBuffer->GetAlbedoMetalness();
        auto* normalRough = m_gBuffer->GetNormalRoughness();
        auto* position    = m_gBuffer->GetPosition();
        auto* depthBuffer = m_gBuffer->GetDepth();

        // 1. Barreras de transición hacia estado de escritura
        graphicsContext->TransitionResource(albedoMetal, ResourceState::RenderTarget);
        graphicsContext->TransitionResource(normalRough, ResourceState::RenderTarget);
        graphicsContext->TransitionResource(position,    ResourceState::RenderTarget);
        graphicsContext->TransitionResource(depthBuffer, ResourceState::DepthWrite);

        // 2. Limpieza de los 3 Render Targets y Depth Buffer
        Math::Color zeroClear = { 0.0f, 0.0f, 0.0f, 0.0f };
        graphicsContext->ClearRenderTarget(albedoMetal, zeroClear);
        graphicsContext->ClearRenderTarget(normalRough, zeroClear);
        graphicsContext->ClearRenderTarget(position,    zeroClear);
        graphicsContext->ClearDepth(depthBuffer, 1.0f, 0);

        // 3. Enlazar MRT + DSV
        ColorBuffer* gbufferTargets[] = { albedoMetal, normalRough, position };
        graphicsContext->SetRenderTargets(3, gbufferTargets, depthBuffer);

        graphicsContext->SetPipelineState(m_gbufferPSO.get());
        graphicsContext->SetRootSignature(m_gbufferRootSig.get());
        graphicsContext->SetPrimitiveTopology(PrimitiveTopology::TriangleList);

        // 4. Vincular tablas Bindless (t0 = Materiales, t1 = Texturas)
        graphicsContext->SetDescriptorTable(2, m_materialManager->GetSRVAllocation());
        graphicsContext->SetDescriptorTable(3, m_device->GetSRVDescriptorAllocator());

        const Math::Matrix viewProjTransposed = camera.GetViewProjection().Transpose();
        VertexBuffer* currentVB = nullptr;

        // 5. Dibujar entidades visibles
        const auto& visibleEntities = cullingData.MainCamera;
        for (auto entt : visibleEntities)
        {
            Entity entity{ entt, m_scene.get() };
            const auto& modelComponent = entity.GetComponent<ModelComponent>();
            const auto& transformComponent = entity.GetComponent<TransformComponent>();

            if (modelComponent.RuntimeMaterialIndex == 0xFFFFFFFF)
            {
                continue;
            }

            auto model = m_assetManager->GetAsset<Model>(modelComponent.ModelHandle);
            auto& mesh = model->GetMeshes()[modelComponent.MeshIndex];

            auto& vertexBuffer = model->GetVertexBuffer();
            auto& indexBuffer  = model->GetIndexBuffer();
            if (currentVB != vertexBuffer.get())
            {
                graphicsContext->SetVertexBuffer(0, vertexBuffer.get());
                graphicsContext->SetIndexBuffer(indexBuffer.get());
                currentVB = vertexBuffer.get();
            }

            // b0: Transformaciones (VS)
            SceneObjectBuffer objConstants;
            objConstants.World = transformComponent.WorldTransform.Transpose();
            objConstants.ViewProjection = viewProjTransposed;

            ConstantBuffer<SceneObjectBuffer> transformCB;
            transformCB.Update(*graphicsContext, objConstants);
            graphicsContext->SetConstantBuffer(0, transformCB);

            // b1: ID Bindless del Material (PS)
            graphicsContext->SetPushConstant(1, modelComponent.RuntimeMaterialIndex, 0);

            graphicsContext->DrawIndexedInstanced(
                mesh->GetIndexCount(),
                1,
                mesh->GetBaseIndex(),
                mesh->GetBaseVertex(),
                0
            );

            Profiler::Get().Stats.DrawCalls++;
            Profiler::Get().Stats.TriangleCount += (mesh->GetIndexCount() / 3);
        }
    }

    void SceneRendererDeferred::RenderDirectionalLightPass(GraphicsContext* graphicsContext, Camera& camera, ColorBuffer* targetRenderTarget, const std::vector<CascadeData>& cascades)
    {
        // 1. Transicionar G-Buffer y ShadowMapArray a lectura en Pixel Shader
        graphicsContext->TransitionResource(m_gBuffer->GetAlbedoMetalness(), ResourceState::PixelShaderResource);
        graphicsContext->TransitionResource(m_gBuffer->GetNormalRoughness(), ResourceState::PixelShaderResource);
        graphicsContext->TransitionResource(m_gBuffer->GetPosition(),        ResourceState::PixelShaderResource);
        graphicsContext->TransitionResource(m_shadowMapArray.get(),          ResourceState::PixelShaderResource);
        graphicsContext->TransitionResource(targetRenderTarget,              ResourceState::RenderTarget);

        // 2. Restaurar el Render Target principal (sin Depth Buffer)
        graphicsContext->SetRenderTargets(1, &targetRenderTarget, nullptr);

        graphicsContext->SetPipelineState(m_deferredLightingPSO.get());
        graphicsContext->SetRootSignature(m_deferredLightingRootSig.get());
        graphicsContext->SetPrimitiveTopology(PrimitiveTopology::TriangleList);

        // 3. Rellenar Constant Buffer de Iluminación Direccional y Cascadas (misma estructura que ForwardRenderer)
        DeferredLightingBuffer lightData = {};
        lightData.CameraPosition = camera.GetPosition();

        Math::Vector3 camForward = camera.GetViewInverse().Forward();
        camForward.Normalize();
        lightData.CameraForward = camForward;

        lightData.GlobalAmbientColor  = Math::Vector3(0.05f, 0.05f, 0.05f);
        lightData.NumCascades         = m_shadowMapArray->GetNumCascades();
        lightData.FilterQuality       = static_cast<uint32_t>(m_shadowProperties->m_filterQuality);
        lightData.ShadowLightSize     = m_shadowProperties->m_ShadowLightSize;
        lightData.ShadowMapResolution = static_cast<float>(m_shadowProperties->m_resolution);
        lightData.NormalBiasScale     = 1.0f;
        
        const float baseTexel = 1.0f / (std::max)(lightData.ShadowMapResolution, 1.0f);
        lightData.CascadeTexelBias      = Math::Vector4(baseTexel, baseTexel * 1.5f, baseTexel * 2.0f, baseTexel * 3.0f);
        lightData.CascadeWorldTexelSize = Math::Vector4(baseTexel, baseTexel, baseTexel, baseTexel);
        
        auto entitiesLightsGroup = m_scene->GetAllEntitiesWith<TransformComponent, DirectionalLightComponent>();
        for (auto lightEntity : entitiesLightsGroup)
        {
            auto [transformComponent, directionalLight] = entitiesLightsGroup.get<TransformComponent, DirectionalLightComponent>(lightEntity);

            lightData.Sun.Direction = directionalLight.WorldDirection;
            lightData.Sun.Intensity = directionalLight.Intensity;
            lightData.Sun.Color     = directionalLight.Color;

            float splitsDistances[4] = {};
            for (uint32_t i = 0; i < lightData.NumCascades; ++i)
            {
                lightData.LightSpaceMatrices[i] = cascades[i].LightViewProj.Transpose();
                splitsDistances[i] = cascades[i].SplitDistance;
            }
            lightData.CascadeSplits = Math::Vector4(
                splitsDistances[0],
                splitsDistances[1],
                splitsDistances[2],
                splitsDistances[3]
            );
            break;
        }

        m_deferredLightsCB.Update(*graphicsContext, lightData);

        // 4. Enlazar b0, t0 (AlbedoMetal), t1 (NormalRough), t2 (Position) y t3 (ShadowMapArray)
        graphicsContext->SetConstantBuffer(0, m_deferredLightsCB);
        graphicsContext->SetDescriptorTable(1, m_gBuffer->GetAlbedoMetalness()->GetSVRAllocation());
        graphicsContext->SetDescriptorTable(2, m_gBuffer->GetNormalRoughness()->GetSVRAllocation());
        graphicsContext->SetDescriptorTable(3, m_gBuffer->GetPosition()->GetSVRAllocation());
        graphicsContext->SetDescriptorTable(4, m_shadowMapArray->GetSVRAllocation());

        // 5. Triángulo procedural a pantalla completa (SV_VertexID)
        graphicsContext->DrawInstanced(3, 1, 0, 0);
        Profiler::Get().Stats.DrawCalls++;
    }

    void SceneRendererDeferred::RenderLocalLightsPass(GraphicsContext* graphicsContext, Camera& camera, ColorBuffer* targetRenderTarget)
    {
        m_outsidePointLights.clear();
        m_insidePointLights.clear();
        m_outsideSpotLights.clear();
        m_insideSpotLights.clear();

        const Math::Vector3 camPos = camera.GetPosition();
        const float nearSafetyMargin = camera.GetNearPlane() * 2.0f;

        // 1. Recolectar y clasificar Point Lights desde EnTT
        auto pointView = m_scene->GetAllEntitiesWith<TransformComponent, PointLightComponent>();
        for (auto entity : pointView)
        {
            const auto& [transform, light] = pointView.get<TransformComponent, PointLightComponent>(entity);
            if (light.Intensity <= 0.0f || light.Radius <= 0.0f)
            {
                continue;
            }

            Math::Vector3 lightPos = transform.WorldTransform.Translation();
            LightInstanceData inst = {};
            inst.World     = (Math::Matrix::CreateScale(light.Radius) * Math::Matrix::CreateTranslation(lightPos)).Transpose();
            inst.Position  = lightPos;
            inst.Radius    = light.Radius;
            inst.Color     = light.Color * light.Intensity;
            inst.LightType = 0;

            float distToCam = Math::Vector3::Distance(camPos, lightPos);
            if (distToCam < (light.Radius + nearSafetyMargin))
            {
                m_insidePointLights.push_back(inst);
            }
            else
            {
                m_outsidePointLights.push_back(inst);
            }
        }

        // 2. Recolectar y clasificar Spot Lights desde EnTT
        auto spotView = m_scene->GetAllEntitiesWith<TransformComponent, SpotLightComponent>();
        for (auto entity : spotView)
        {
            const auto& [transform, light] = spotView.get<TransformComponent, SpotLightComponent>(entity);
            if (light.Intensity <= 0.0f || light.Range <= 0.0f)
            {
                continue;
            }

            Math::Vector3 lightPos = transform.WorldTransform.Translation();
            Math::Vector3 lightDir = transform.WorldTransform.Forward();
            lightDir.Normalize();

            float outerRadius = light.Range * std::tan(light.OuterCutoffAngle);
            Math::Matrix scaleMat = Math::Matrix::CreateScale(outerRadius, outerRadius, light.Range);
            Math::Matrix rotTrans = transform.WorldTransform;
            rotTrans.Translation(lightPos);

            LightInstanceData inst = {};
            inst.World        = (scaleMat * rotTrans).Transpose();
            inst.Position     = lightPos;
            inst.Radius       = light.Range;
            inst.Color        = light.Color * light.Intensity;
            inst.LightType    = 1;
            inst.Direction    = lightDir;
            inst.InnerConeCos = std::cos(light.InnerCutoffAngle);
            inst.OuterConeCos = std::cos(light.OuterCutoffAngle);

            // Margen conservador basado en el alcance del cono
            float distToCam = Math::Vector3::Distance(camPos, lightPos);
            if (distToCam < (light.Range + nearSafetyMargin))
            {
                m_insideSpotLights.push_back(inst);
            }
            else
            {
                m_outsideSpotLights.push_back(inst);
            }
        }

        if (m_outsidePointLights.empty() && m_insidePointLights.empty() &&
            m_outsideSpotLights.empty() && m_insideSpotLights.empty())
        {
            return;
        }

        // 3. Configurar estado común de Luces Locales (Depth Read-Only para Early-Z)
        graphicsContext->TransitionResource(m_gBuffer->GetDepth(), ResourceState::DepthRead);
        graphicsContext->SetRenderTargets(1, &targetRenderTarget, m_gBuffer->GetDepth());
        graphicsContext->SetRootSignature(m_localLightsRootSig.get());
        graphicsContext->SetPrimitiveTopology(PrimitiveTopology::TriangleList);

        LocalLightsCameraBuffer camData = {};
        camData.ViewProjection = camera.GetViewProjection().Transpose();
        camData.CameraPosition = camPos;
        m_localLightsCameraCB.Update(*graphicsContext, camData);

        graphicsContext->SetConstantBuffer(0, m_localLightsCameraCB);
        graphicsContext->SetDescriptorTable(2, m_gBuffer->GetAlbedoMetalness()->GetSVRAllocation());
        graphicsContext->SetDescriptorTable(3, m_gBuffer->GetNormalRoughness()->GetSVRAllocation());
        graphicsContext->SetDescriptorTable(4, m_gBuffer->GetPosition()->GetSVRAllocation());

        auto dispatchLightBatch = [&](const std::vector<LightInstanceData>& batch, GraphicsPipelineState* pso, VertexBuffer* vb, IndexBuffer* ib, uint32_t indexCount)
        {
            if (batch.empty() || !vb || !ib || indexCount == 0)
            {
                return;
            }

            graphicsContext->SetPipelineState(pso);
            graphicsContext->SetVertexBuffer(0, vb);
            graphicsContext->SetIndexBuffer(ib);

            size_t allocSize = batch.size() * sizeof(LightInstanceData);
            DynamicAllocation alloc = graphicsContext->AllocateDynamicSpace(allocSize);
            std::memcpy(alloc.CPUAddress, batch.data(), allocSize);

            graphicsContext->SetShaderResourceView(1, alloc);
            graphicsContext->DrawIndexedInstanced(indexCount, static_cast<uint32_t>(batch.size()), 0, 0, 0);

            Profiler::Get().Stats.DrawCalls++;
            Profiler::Get().Stats.TriangleCount += (indexCount / 3) * static_cast<uint32_t>(batch.size());
        };

        // 4. Despachar máximo 4 Draw Calls para TODAS las luces de la escena
        dispatchLightBatch(m_outsidePointLights, m_localLightOutsidePSO.get(), m_unitSphereVB.get(), m_unitSphereIB.get(), m_unitSphereIB->GetIndicesCount());
        dispatchLightBatch(m_insidePointLights,  m_localLightInsidePSO.get(),  m_unitSphereVB.get(), m_unitSphereIB.get(), m_unitSphereIB->GetIndicesCount());
        dispatchLightBatch(m_outsideSpotLights,  m_localLightOutsidePSO.get(), m_unitConeVB.get(),   m_unitConeIB.get(),   m_unitConeIB->GetIndicesCount());
        dispatchLightBatch(m_insideSpotLights,   m_localLightInsidePSO.get(),  m_unitConeVB.get(),   m_unitConeIB.get(),   m_unitConeIB->GetIndicesCount());
    }
}
