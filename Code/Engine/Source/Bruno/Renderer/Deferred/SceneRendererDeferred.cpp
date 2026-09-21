#include "brpch.h"
#include "SceneRendererDeferred.h"

#include "GBuffer.h"
#include "Bruno/Platform/DirectX/DepthBuffer.h"
#include "Bruno/Platform/DirectX/GraphicsDevice.h"
#include "Bruno/Platform/DirectX/GraphicsPipelineState.h"
#include "Bruno/Platform/DirectX/RootSignature.h"
#include "Bruno/Platform/DirectX/VertexTypes.h"
#include "Bruno/Renderer/PSOCache.h"
#include "Bruno/Renderer/RootSignatureLibrary.h"
#include "Bruno/Platform/DirectX/Texture2D.h"

namespace Bruno
{
    SceneRendererDeferred::SceneRendererDeferred(GraphicsDevice* device, std::shared_ptr<Scene> scene) :
        m_scene(scene)
    {
    }

    void SceneRendererDeferred::Render(GraphicsContext* context, Camera& camera, uint32_t frameIndex)
    {
    }

    void SceneRendererDeferred::Resize(uint32_t width, uint32_t height)
    {
    }
    
    void SceneRendererDeferred::InitializeGBuffer(GraphicsDevice* device)
    {
        m_gBuffer = std::make_shared<GBuffer>();
        m_gBuffer->Initialize(*device, device->GetSRVDescriptorAllocator(), device->GetRTVDescriptorAllocator(), 100, 100);
    }

    void SceneRendererDeferred::InitializeGBufferRootSignature(GraphicsDevice* device)
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

    void SceneRendererDeferred::InitializeDeferredRootSignature(GraphicsDevice* device)
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

    void SceneRendererDeferred::InitializeDeferredPSOs(GraphicsDevice* device)
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
}
