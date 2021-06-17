#include "stdafx.h"
#include "Renderer.h"

#include "Scene.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/EffectParameter.h"
#include "Graphics/TextureCollection.h"
#include "Graphics/SamplerState.h"
#include "Graphics/RasterizerState.h"
#include "Graphics/DepthStencilState.h"
#include "Graphics/BlendState.h"
#include "Graphics/Models/Model.h"

namespace TrioEngine
{
	GraphicsDevice* Renderer::g_device = nullptr;
	std::unordered_map<const Camera*, FrameCulling> Renderer::g_frameCullings;

	void Renderer::DrawScene(EffectParameter* parameter, Matrix& viewProjection /*const Camera& camera*/)
	{
		Scene& scene = *Scene::GetActiveScene();
		GraphicsDevice* device = g_device;
		//scene.Update();

		for (size_t i = 0; i < scene.GetMeshes().GetCount(); i++)
		{
			MeshComponent& mesh = scene.GetMeshes()[i];
			Entity meshEntity = scene.GetMeshes().GetEntity(i);

			device->SetVertexBuffer(&mesh.m_vertexBuffer);
			device->SetIndexBuffer(&mesh.m_indexBuffer);
			TransformComponent& transform = *scene.GetTransforms().GetComponent(meshEntity);
			
			for (size_t j = 0; j < mesh.m_subMeshes.size(); j++)
			{
				auto& subMesh = mesh.m_subMeshes[j];

				if (subMesh.m_vertexCount == 0) continue;

				MaterialComponent& material = *scene.GetMaterials().GetComponent(subMesh.m_materialId);

				device->GetTextures()->SetTexture(0, material.diffuseTexture);
				device->SetSamplerState(0, SamplerState::LinearWrap);
				device->SetRasterizerState(RasterizerState::CullCounterClockwise);
				device->SetDepthStencilState(DepthStencilState::Default);
				device->SetBlendState(BlendState::Opaque);

				Matrix meshWorld = transform.m_world * viewProjection;

				parameter->SetValue(meshWorld);
				device->DrawIndexedPrimitives(PrimitiveType::TriangleList, subMesh.m_vertexOffset, subMesh.m_startIndex, subMesh.m_primitiveCount);
			}
		}
	}

	void Renderer::Initialize(GraphicsDevice* device)
	{
		g_device = device;

		Camera* camera = Scene::GetCamera();
		g_frameCullings[camera].Clear();
	}

	void Renderer::UpdatePerFrameData()
	{

		PerformFrameCulling();
	}

	void Renderer::PerformFrameCulling()
	{

	}

	GraphicsDevice* Renderer::GetDevice()
	{
		return g_device;
	}
}