#include "brpch.h"
#include "SceneRenderer.h"

#include "PrimitiveBatch.h"
#include "Bruno/Scene/Scene.h"
#include "Bruno/Renderer/RenderItem.h"
#include "Bruno/Renderer/Material.h"

#include "Bruno/Platform/DirectX/Texture2D.h"
#include "Bruno/Platform/DirectX/GraphicsContext.h"
#include "Bruno/Platform/DirectX/RootSignature.h"
#include "Bruno/Platform/DirectX/Shader.h"

#include "Bruno/Content/AssetManager.h"
#include "Bruno/Core/Memory.h"
#include "Bruno/Platform/DirectX/ShaderCompiler.h"
#include "Bruno/Platform/DirectX/VertexTypes.h"
#include "Bruno/Renderer/Camera.h"
#include "Bruno/Scene/Systems/FrustumCulling.h"

namespace Bruno
{
	SceneRenderer::SceneRenderer(std::shared_ptr<Scene> scene, std::shared_ptr<FrustumCulling> frustumCulling, AbstractAssetManager* assetManager) :
		m_scene(scene),
		m_frustumCulling(frustumCulling),
		m_assetManager(assetManager)
	{
		auto& device = Bruno::Graphics::GetDevice();
		
		InitializeOpaqueRootSignature(device);

		InitializeOpaquePSO(device);
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
	
	void SceneRenderer::InitializeOpaqueRootSignature(GraphicsDevice* device)
	{
		// 1. Configuramos los rangos (Textura)
		CD3DX12_DESCRIPTOR_RANGE srvTable;
		srvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

		// 2. Configuramos los parámetros (Matriz y Textura)
		CD3DX12_ROOT_PARAMETER params[2];
		params[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
		params[1].InitAsDescriptorTable(1, &srvTable, D3D12_SHADER_VISIBILITY_PIXEL);

		// 3. Sampler
		CD3DX12_STATIC_SAMPLER_DESC sampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);

		// 4. Inicializamos nuestra Root Signature
		m_opaqueRootSignature = std::make_shared<RootSignature>(*device);
		m_opaqueRootSignature->Initialize(2, params, 1, &sampler);
	}

	void SceneRenderer::InitializeOpaquePSO(GraphicsDevice* device)
	{
		// Instanciamos el Pipeline State Object (PSO) pasándole el contrato y shaders
		ShaderCompiler compiler; 

		auto vertexShaderByteCode = compiler.CompileFromFile(L"Shaders/Opaque.hlsl", L"VS", L"vs_6_0");
		auto pixelShaderByteCode  = compiler.CompileFromFile(L"Shaders/Opaque.hlsl", L"PS", L"ps_6_0");
		
		std::unique_ptr<ShaderProgram> vertexShader = std::make_unique<ShaderProgram>(ShaderStage::Vertex, vertexShaderByteCode);
		std::unique_ptr<ShaderProgram> pixelShader = std::make_unique<ShaderProgram>(ShaderStage::Pixel, pixelShaderByteCode);
		
		GraphicsPipelineStateDesc psoDesc = {};
		// Definir el Input Layout (DEBE COINCIDIR CON ModelVertex Y CON EL HLSL)
		psoDesc.RootSignature = m_opaqueRootSignature.get();
		psoDesc.InputLayout = VertexPositionNormalTexture::GetLayout();
		
        psoDesc.VertexShader = vertexShader.get();
        psoDesc.PixelShader = pixelShader.get();
		
		psoDesc.RasterizerState.CullMode = CullMode::Back;
		psoDesc.RasterizerState.FillMode = FillMode::Solid;
		psoDesc.RasterizerState.FrontCounterClockwise = true;
        
        psoDesc.Topology = PrimitiveTopology::TriangleList;
    
        // Formatos de Salida (DEBEN coincidir con tu SwapChain y DepthBuffer)
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = TextureFormat::R8G8B8A8_Unorm;
        psoDesc.DSVFormat = TextureFormat::D24_Unorm_S8_Uint;
		
		m_opaquePSO = std::make_shared<GraphicsPipelineState>(*device);
		m_opaquePSO->Initialize(psoDesc);
	}

	void SceneRenderer::OnRender(GraphicsContext* graphicsContext, Camera& camera, uint32_t frameIndex)
	{
		m_frustumCulling->Update();
		auto& visibleEntities = m_frustumCulling->GetVisibleEntities();
		
		VertexBuffer* currentVB = nullptr;
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
				
				graphicsContext->SetPipelineState(material->GetPSO().get());
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
			}
		}
	}

}
