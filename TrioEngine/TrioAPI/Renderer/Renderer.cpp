#include "stdafx.h"
#include "Renderer.h"

#include "Scene.h"
#include "Graphics/GraphicsDevice.h"

namespace TrioEngine
{
	GraphicsDevice* Renderer::g_device = nullptr;
	std::unordered_map<const Camera*, FrameCulling> Renderer::g_frameCullings;

	void Renderer::DrawScene(/*const Camera& camera*/)
	{
		Scene& scene = *Scene::GetActiveScene();
		GraphicsDevice* device = g_device;

		for (size_t i = 0; i < scene.GetMeshes().GetCount(); i++)
		{
			MeshComponent& mesh = scene.GetMeshes()[i];

			device->SetVertexBuffer(&mesh.m_vertexBuffer);
			device->SetIndexBuffer(&mesh.m_indexBuffer);

			for (size_t j = 0; j < mesh.m_subMeshes.size(); j++)
			{
				auto& subMesh = mesh.m_subMeshes[j];

				if (subMesh.m_vertexCount == 0) continue;

				device->DrawIndexedPrimitives(PrimitiveType::TriangleList, subMesh.m_vertexOffset, subMesh.m_startIndex, subMesh.m_primitiveCount);
			}
		}
	}

	void Renderer::Initialize(GraphicsDevice* device)
	{
		g_device = device;

		Camera& camera = *Scene::GetCamera();
		g_frameCullings[&camera].Clear();
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