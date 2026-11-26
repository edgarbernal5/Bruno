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
        float Padding[3];
    };
    static_assert(sizeof(LightInstanceData) == 128, "Desajuste de stride en LightInstanceData.");
    struct LocalLightsCameraBuffer
    {
        Math::Matrix ViewProjection;
        Math::Vector3 CameraPosition;
        float Padding;
    };
    
    struct alignas(16) DeferredLightingBuffer
    {
        DirectionalLightData Sun;                     // offset 0   (32 B)

        Math::Vector3        GlobalAmbientColor;      // offset 32  (12 B)
        float                AmbientPadding;          // offset 44  (4 B)

        Math::Vector3        CameraPosition;          // offset 48  (12 B)
        float                CameraPadding;           // offset 60  (4 B)

        Math::Vector3        CameraForward;           // offset 64  (12 B)
        uint32_t             NumCascades;             // offset 76  (4 B)

        Math::Matrix         LightSpaceMatrices[4];   // offset 80  (256 B)
        Math::Vector4        CascadeSplits;           // offset 336 (16 B)

        uint32_t             FilterQuality;           // offset 352 (4 B)
        float                ShadowMapResolution;     // offset 356 (4 B)
        float                ShadowLightSize;         // offset 360 (4 B)
        float                NormalBiasScale;         // offset 364 (4 B)

        Math::Vector4        CascadeTexelBias;        // offset 368 (16 B)
        Math::Vector4        CascadeWorldTexelSize;   // offset 384 (16 B)
    };
    
    static_assert(sizeof(DeferredLightingBuffer) == 400, "DeferredLightingBuffer debe medir 400 bytes igual que en DeferredLighting.hlsl.");
    
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
