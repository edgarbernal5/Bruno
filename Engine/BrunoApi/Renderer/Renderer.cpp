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

namespace BrunoEngine
{
	GraphicsDevice* Renderer::g_device = nullptr;
	std::unordered_map<const Camera*, FrameCulling> Renderer::g_frameCullings;

	void Renderer::DrawScene(EffectParameter* mvpParameter, EffectParameter* modelParameter, Matrix& viewProjection /*const Camera& camera*/)
	{
		Scene& scene = *Scene::GetActiveScene();

		g_device->SetSamplerState(0, SamplerState::LinearWrap);
		g_device->SetRasterizerState(RasterizerState::CullCounterClockwise);
		g_device->SetDepthStencilState(DepthStencilState::Default);
		g_device->SetBlendState(BlendState::Opaque);

		for (size_t i = 0; i < scene.GetMeshes().GetCount(); i++)
		{
			MeshComponent& mesh = scene.GetMeshes()[i];
			Entity meshEntity = scene.GetMeshes().GetEntity(i);

			g_device->SetVertexBuffer(&mesh.m_vertexBuffer);
			g_device->SetIndexBuffer(&mesh.m_indexBuffer);
			TransformComponent& transform = *scene.GetTransforms().GetComponent(meshEntity);

			Matrix meshWorld = transform.m_world * viewProjection;
			mvpParameter->SetValue(meshWorld);

			modelParameter->SetValue(transform.m_world);

			for (size_t j = 0; j < mesh.m_subMeshes.size(); j++)
			{
				auto& subMesh = mesh.m_subMeshes[j];

				if (subMesh.m_vertexCount == 0) continue;

				MaterialComponent& material = *scene.GetMaterials().GetComponent(subMesh.m_materialId);

				g_device->GetTextures()->SetTexture(0, material.diffuseTexture);

				g_device->DrawIndexedPrimitives(PrimitiveType::TriangleList, subMesh.m_vertexOffset, subMesh.m_startIndex, subMesh.m_primitiveCount);
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