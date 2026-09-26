#pragma once
#include "Bruno/Math/Math.h"
#include "Bruno/Platform/DirectX/ConstantBuffer.h"

namespace Bruno
{
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
        Math::Vector3 Padding; // Relleno para 16-bytes
    };

    struct ForwardLightingBuffer
    {
        DirectionalLightData Sun; // La luz direccional global
        PointLightData PointLights[MAX_FORWARD_LIGHTS]; // Tu arreglo actual de luces locales
        SpotLightData SpotLights[MAX_FORWARD_LIGHTS]; // Tu arreglo actual de luces locales
        Math::Vector3 GlobalAmbientColor;
        uint32_t ActivePointLightCount;
        uint32_t ActiveSpotLightCount;
        Math::Vector3 CameraPosition;
        float Padding;
    };
    
    class ForwardRenderer
    {
    public:
        ForwardRenderer(GraphicsDevice* device, std::shared_ptr<Scene> scene, std::shared_ptr<MaterialManager> materialManager);
        
        void Render(GraphicsContext* graphicsContext, Camera& camera, uint32_t frameIndex, const FrameCullingResults& cullingData);
		
    private:
        void InitializeForwardRootSignature(GraphicsDevice* device);
        void InitializeForwardPSO(GraphicsDevice* device);
        
        std::shared_ptr<Scene> m_scene;
        std::unique_ptr<Shader> m_opaqueShader;
		
        DescriptorAllocator* m_globalSrvHeap;
        std::shared_ptr<RootSignature> m_forwardRootSig;
        std::shared_ptr<GraphicsPipelineState> m_forwardPSO;
        std::shared_ptr<MaterialManager> m_materialManager;
        
        Math::Vector3 m_directionalLightDir { 1.0f, 0.0f, 0.0f};
        ConstantBuffer<ForwardLightingBuffer> m_forwardLightsCB;
    };
}
