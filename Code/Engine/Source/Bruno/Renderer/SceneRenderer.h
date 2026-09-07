#pragma once

#include <Bruno/Platform/DirectX/RootSignature.h>
#include <Bruno/Platform/DirectX/GraphicsPipelineState.h>
#include <Bruno/Math/Math.h>

#include "Bruno/Platform/DirectX/ConstantBuffer.h"

namespace entt
{
	enum class entity : std::uint32_t;
}

namespace Bruno
{
	class MaterialManager;
	class Material;
	class DescriptorAllocator;
	class GBuffer;
	class FrustumCulling;
	class GraphicsContext;
	class Scene;
	class Shader;
	class AbstractAssetManager;
	class Camera;
	
	// Datos Puros de Iluminación
#define MAX_FORWARD_LIGHTS 8

	struct ForwardLightData
	{
		Math::Vector3 Position;
		float Radius;
		Math::Vector3 Color;
		float Intensity;
	};

	struct ForwardLightingBuffer
	{
		ForwardLightData Lights[MAX_FORWARD_LIGHTS];
		Math::Vector3 GlobalAmbientColor;
		uint32_t ActiveLightCount;
    
		// ¡La cámara se muda a la iluminación!
		Math::Vector3 CameraPosition; 
		float Padding; // Relleno obligatorio de 4 bytes para cerrar el registro de 16 bytes
	};

	class SceneRenderer
	{
	public:
		SceneRenderer(std::shared_ptr<Scene> scene, std::shared_ptr<FrustumCulling> frustumCulling, AbstractAssetManager* assetManager);
		~SceneRenderer();
		
		// Se llama cuando cargas una escena o agregas un objeto
		void InitEntitiesForRender();
		
		void RenderForward(GraphicsContext* graphicsContext, Camera& camera, uint32_t frameIndex);
		void RenderDeferred(GraphicsContext* context, Camera& camera, uint32_t frameIndex);
		
		void Resize(uint32_t width, uint32_t height);
		
	private:
		void InitializeGBuffer(GraphicsDevice* device);
		
		void InitializeForwardRootSignature(GraphicsDevice* device);
		void InitializeForwardPSO(GraphicsDevice* device);
		
		void InitializeGBufferRootSignature(GraphicsDevice* device);
		void InitializeDeferredRootSignature(GraphicsDevice* device);
		void InitializeDeferredPSOs(GraphicsDevice* device);
		void InitializeShadowPipeline(GraphicsDevice* device);
		
		void RegisterMaterialToGPU(std::shared_ptr<Material> matAsset);
		void DrawBatch(GraphicsContext* graphicsContext, const std::vector<entt::entity>& visibleEntities);
		
		std::shared_ptr<Scene> m_scene;
		std::shared_ptr<FrustumCulling> m_frustumCulling;
		AbstractAssetManager* m_assetManager;

		DescriptorAllocator* m_globalSrvHeap;
		std::shared_ptr<GBuffer> m_gBuffer;
		std::shared_ptr<RootSignature> m_forwardRootSig;
		std::shared_ptr<RootSignature> m_shadowRootSig;
		std::shared_ptr<RootSignature> m_gbufferRootSig;
		std::shared_ptr<RootSignature> m_deferredLightingRootSig;
		
		std::unique_ptr<MaterialManager> m_materialManager;
		std::unique_ptr<Shader> m_opaqueShader;
		
		std::shared_ptr<GraphicsPipelineState> m_forwardPSO;
		std::shared_ptr<GraphicsPipelineState> m_shadowPSO;
		std::shared_ptr<GraphicsPipelineState> m_gbufferPSO;
		std::shared_ptr<GraphicsPipelineState> m_deferredLightingPSO;
		
		ConstantBuffer<ForwardLightingBuffer> m_forwardLightsCB;
	};
}
