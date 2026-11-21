#pragma once

#include "Bruno/Platform/DirectX/ConstantBuffer.h"
#include "Bruno/Renderer/Lighting/EnumTypes.h"
#include "Bruno/Renderer/Shadows/EnumTypes.h"

namespace Bruno
{
    class ColorBuffer;
    struct CascadeData;
    struct FrameCullingResults;
    struct ShadowProperties;
    class ShadowMapArray;
    class MaterialManager;
    class Scene;
    class Camera;
    class GraphicsContext;
    class GraphicsPipelineState;
    class RootSignature;
    class GBuffer;
    
    // Alineado a 16 bytes para StructuredBuffer<LightInstanceData> en HLSL
    struct alignas(16) LightInstanceData
    {
        Math::Matrix World;
        Math::Vector3 Position;
        float Radius;
        Math::Vector3 Color;
        uint32_t LightType; // 0 = PointLight, 1 = SpotLight
        Math::Vector3 Direction;
        float InnerConeCos;
        float OuterConeCos;
        float Padding;
    };

    struct LocalLightsCameraBuffer
    {
        Math::Matrix ViewProjection;
        Math::Vector3 CameraPosition;
        float Padding;
    };
    
    struct DeferredLightingBuffer
    {
        DirectionalLightData Sun;
        Math::Vector3 GlobalAmbientColor;
        uint32_t ActivePointLightCount;
        uint32_t ActiveSpotLightCount;
        Math::Vector3 CameraPosition;
        float Padding;
        Math::Vector3 CameraForward;
        
        Math::Matrix LightSpaceMatrices[4];
        Math::Vector4 CascadeSplits; // [Split0, Split1, Split2, Split3]
        uint32_t NumCascades;
        
        // --- VARIABLES INYECTADAS DESDE C++ (ShadowProperties) ---
        uint32_t FilterQuality;
        float ShadowMapResolution;
        float ShadowLightSize;     // Tamaño aparente de la luz para PCSS (ej: 0.05)
        //float CsmPadding;          // Padding de alineación
        Math::Vector4 CascadeTexelBias;
    };
    
    class SceneRendererDeferred
    {
    public:
        SceneRendererDeferred(
            GraphicsDevice* device,
            std::shared_ptr<Scene> scene,
            std::shared_ptr<MaterialManager> materialManager,
            std::shared_ptr<ShadowMapArray> shadowMapArray,
            AbstractAssetManager* assetManager,
            ShadowProperties* shadowProperties,
            uint32_t initialWidth = 1280,
            uint32_t initialHeight = 720
        );
        ~SceneRendererDeferred() = default;
        
        void Render(GraphicsContext* graphicsContext, Camera& camera, uint32_t frameIndex, ColorBuffer* targetRenderTarget, const std::vector<CascadeData>& cascades, const FrameCullingResults& cullingData);
		
        void Resize(uint32_t width, uint32_t height);
    private:
        void InitializeGBuffer(GraphicsDevice* device, uint32_t width, uint32_t height);
        void InitializeGBufferRootSignature(GraphicsDevice* device);
        void InitializeDeferredRootSignature(GraphicsDevice* device);
        void InitializeLocalLightsRootSignature(GraphicsDevice* device);
        void InitializeDeferredPSOs(GraphicsDevice* device);
        void InitializeProxyMeshes(GraphicsDevice* device);

        void RenderGBufferPass(GraphicsContext* graphicsContext, Camera& camera, const FrameCullingResults& cullingData);
        void RenderDirectionalLightPass(GraphicsContext* graphicsContext, Camera& camera, ColorBuffer* targetRenderTarget, const std::vector<CascadeData>& cascades);
        void RenderLocalLightsPass(GraphicsContext* graphicsContext, Camera& camera, ColorBuffer* targetRenderTarget);

        GraphicsDevice* m_device{ nullptr };
        std::shared_ptr<Scene> m_scene;
        std::shared_ptr<MaterialManager> m_materialManager;
        std::shared_ptr<ShadowMapArray> m_shadowMapArray;
        AbstractAssetManager* m_assetManager{ nullptr };
        ShadowProperties* m_shadowProperties{ nullptr };
        DescriptorAllocator* m_globalSrvHeap{ nullptr };

        std::shared_ptr<GBuffer> m_gBuffer;

        // Root Signatures
        std::shared_ptr<RootSignature> m_gbufferRootSig;
        std::shared_ptr<RootSignature> m_deferredLightingRootSig;
        std::shared_ptr<RootSignature> m_localLightsRootSig;

        // Pipeline State Objects
        std::shared_ptr<GraphicsPipelineState> m_gbufferPSO;
        std::shared_ptr<GraphicsPipelineState> m_deferredLightingPSO;
        std::shared_ptr<GraphicsPipelineState> m_localLightOutsidePSO;
        std::shared_ptr<GraphicsPipelineState> m_localLightInsidePSO;

        // Constant Buffers
        ConstantBuffer<DeferredLightingBuffer> m_deferredLightsCB;
        ConstantBuffer<LocalLightsCameraBuffer> m_localLightsCameraCB;

        // Mallas Proxy Unitarias en VRAM (VRAM Default Heap)
        std::unique_ptr<VertexBuffer> m_unitSphereVB;
        std::unique_ptr<IndexBuffer> m_unitSphereIB;
        uint32_t m_unitSphereIndexCount{ 0 };

        std::unique_ptr<VertexBuffer> m_unitConeVB;
        std::unique_ptr<IndexBuffer> m_unitConeIB;
        uint32_t m_unitConeIndexCount{ 0 };

        // Listas reutilizables por frame para evitar alocaciones dinámicas
        std::vector<LightInstanceData> m_outsidePointLights;
        std::vector<LightInstanceData> m_insidePointLights;
        std::vector<LightInstanceData> m_outsideSpotLights;
        std::vector<LightInstanceData> m_insideSpotLights;

        ShadowFilterQuality m_lastShadowFilterQuality{ ShadowFilterQuality::PCF3x3 };
        uint32_t m_viewportWidth{ 0 };
        uint32_t m_viewportHeight{ 0 };
    };
}
