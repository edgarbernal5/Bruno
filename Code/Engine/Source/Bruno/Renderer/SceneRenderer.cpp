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
					cbv.TransformCB[i] = std::make_shared<ConstantBuffer>(device->GetNativeDevice().Get(), objectSize);
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

		// Compilas usando DXC (nota el _6_0)
		auto vertexShaderByteCode = compiler.CompileFromFile(L"Shaders/Opaque.hlsl", L"VS", L"vs_6_0");
		auto pixelShaderByteCode  = compiler.CompileFromFile(L"Shaders/Opaque.hlsl", L"PS", L"ps_6_0");
		
		// 1. Definir el Input Layout (DEBE COINCIDIR CON ModelVertex Y CON EL HLSL)
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature = m_opaqueRootSignature->GetNative();
    
        // 2. Adjuntar los Shaders
        psoDesc.VS = { reinterpret_cast<BYTE*>(vertexShaderByteCode->GetBufferPointer()), vertexShaderByteCode->GetBufferSize() };
        psoDesc.PS = { reinterpret_cast<BYTE*>(pixelShaderByteCode->GetBufferPointer()), pixelShaderByteCode->GetBufferSize() };
    
        // 3. Configurar Estados (Usamos los defaults de d3dx12 para código limpio)
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        // Para ver geometría por dentro y por fuera si no tienes backface culling, usa:
        //psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
        
        // ¡La línea mágica que invierte qué lado es el frente!
        psoDesc.RasterizerState.FrontCounterClockwise = TRUE; 
        psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
        // ... (asignas esto a tu D3D12_GRAPHICS_PIPELINE_STATE_DESC)
        
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // Opaco, sin transparencias
        psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // Z-Buffer activado
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    
        // 4. Formatos de Salida (DEBEN coincidir con tu SwapChain y DepthBuffer)
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // O el que uses en tu SwapChain
        psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;  // O el que uses en tu DepthBuffer
        psoDesc.SampleDesc.Count = 1;
        psoDesc.SampleDesc.Quality = 0;
		
		m_opaquePSO = std::make_shared<GraphicsPipelineState>(*device);
		m_opaquePSO->Initialize(psoDesc);
	}

	void SceneRenderer::OnRender(GraphicsContext* graphicsContext, Camera& camera, uint32_t frameIndex)
	{
		VertexBuffer* currentVB = nullptr;
		m_frustumCulling->Update();
		
		auto& visibleEntities = m_frustumCulling->GetVisibleEntities();
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
					graphicsContext->SetVertexBuffer(vertexBuffer->GetView());
					graphicsContext->SetIndexBuffer(&indexBuffer->GetView());
					currentVB = vertexBuffer.get();
				}
				
				graphicsContext->SetPipelineState(material->GetPSO()->GetNative());
				graphicsContext->SetRootSignature(material->GetRootSignature()->GetNative());
				
				// Enlazar la tabla de texturas (Parámetro 1 en nuestra Root Signature)
				graphicsContext->SetDescriptorTable(1, material->GetTextureDescriptorTable());
				
				Math::Matrix world = m_scene->GetWorldSpaceMatrix(entity);
				Math::Matrix wvp = (world * camera.GetViewProjection()).Transpose();
				
				SceneObjectBuffer objConstants;
				objConstants.WorldViewProjection = wvp;
				cbv.TransformCB[frameIndex]->Update(&objConstants, sizeof(SceneObjectBuffer));
				
				graphicsContext->SetConstantBuffer(0, cbv.TransformCB[frameIndex]->GetGPUAddress());
				graphicsContext->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				graphicsContext->DrawIndexedInstanced(mesh->GetIndexCount(),
					1,
					mesh->GetBaseIndex(),
					mesh->GetBaseVertex(),
					0);
			}
		}
	}

}
