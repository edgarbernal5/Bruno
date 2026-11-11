#pragma once
#include "Bruno/Math/Math.h"
#include "Bruno/Platform/DirectX/ConstantBuffer.h"
#include "Bruno/Renderer/Shadows/EnumTypes.h"

namespace Bruno
{
    struct ShadowProperties;
    struct CascadeData;
    class ShadowMapArray;
    class MaterialManager;
    struct FrameCullingResults;
    class RootSignature;
    class GraphicsPipelineState;
    class Scene;
    class Camera;
    class GraphicsContext;

    
    // Datos Puros de Iluminación
#define MAX_FORWARD_LIGHTS 8
	
    struct DirectionalLightData
    {
        Math::Vector3 Direction;
        float Intensity;
        Math::Vector3 Color;
        float Padding; // Relleno obligatorio
    };
	
    struct PointLightData
    {
        Math::Vector3 Position;
        float Radius;
        Math::Vector3 Color;
        float Intensity;
    };
	
    struct SpotLightData
    {
        Math::Vector3 Position;
        float Radius; // Distancia máxima de influencia (como la Point Light)
        Math::Vector3 Direction;
        float Intensity;
        Math::Vector3 Color;
        float InnerConeCos; // std::cos(InnerCutoffAngle)
        float OuterConeCos; // std::cos(OuterCutoffAngle)
        Math::Vector3 Padding;
    };
    
    struct ForwardLightingBuffer
    {
        DirectionalLightData Sun;
        PointLightData PointLights[MAX_FORWARD_LIGHTS];
        SpotLightData SpotLights[MAX_FORWARD_LIGHTS];
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
        float CsmPadding;          // Padding de alineación
    };
    
    class ForwardRenderer
    {
    public:
        ForwardRenderer(GraphicsDevice* device, std::shared_ptr<Scene> scene, std::shared_ptr<MaterialManager> materialManager, std::shared_ptr<ShadowMapArray>, AbstractAssetManager* assetManager, ShadowProperties* shadowProperties);
        
        void Render(GraphicsContext* graphicsContext, Camera& camera, uint32_t frameIndex, const std::vector<CascadeData>& cascades, const FrameCullingResults& cullingData);
		
    private:
        void InitializeForwardRootSignature(GraphicsDevice* device);
        void InitializeForwardPSO(GraphicsDevice* device);
        
        std::shared_ptr<Scene> m_scene;
        std::unique_ptr<Shader> m_opaqueShader;
        AbstractAssetManager* m_assetManager;
        ShadowProperties* m_shadowProperties;
        ShadowFilterQuality m_lastShadowFilterQuality;
        
        DescriptorAllocator* m_globalSrvHeap;
        std::shared_ptr<RootSignature> m_forwardRootSig;
        std::shared_ptr<GraphicsPipelineState> m_forwardPSO;
        std::shared_ptr<MaterialManager> m_materialManager;
        std::shared_ptr<ShadowMapArray> m_shadowMapArray;
        Math::Vector3 m_directionalLightDir { 1.0f, 0.0f, 0.0f};
        ConstantBuffer<ForwardLightingBuffer> m_forwardLightsCB;
    };
}
