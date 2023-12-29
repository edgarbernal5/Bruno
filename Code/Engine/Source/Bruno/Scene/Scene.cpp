#include "brpch.h"
#include "Scene.h"

#include "Bruno/Renderer/Model.h"
#include <Bruno/Renderer/RenderItem.h>
#include <Bruno/Core/GameTimer.h>

namespace Bruno
{
	Scene::Scene(Camera& camera) :
		m_camera(camera)
	{
		m_sceneEntity = m_registry.create();

		for (size_t i = 0; i < Graphics::Core::FRAMES_IN_FLIGHT_COUNT; i++)
		{
			m_objectBuffer[i] = std::make_unique<ConstantBuffer<ObjectBuffer>>();
		}
	}

	void Scene::AddModel(std::shared_ptr<Model> model)
	{
		auto& meshes = model->GetMeshes();
		for (auto& mesh : meshes)
		{
			auto boxRenderItem = std::make_shared<RenderItem>();
			boxRenderItem->IndexCount = mesh->GetIndexCount();
			boxRenderItem->BaseVertexLocation = mesh->GetBaseVertex();
			boxRenderItem->StartIndexLocation = mesh->GetBaseIndex();
			boxRenderItem->Material = model->GetMaterial(mesh->GetMaterialIndex()).get();
			boxRenderItem->IndexBuffer = model->GetIndexBuffer();
			boxRenderItem->VertexBuffer = model->GetVertexBuffer();
			m_renderItems.push_back(boxRenderItem);
		}
		m_transformableObjects.push_back(std::make_shared<Transformable>());

		m_models.push_back(model);
	}

	void Scene::OnUpdate(const GameTimer& timer)
	{
		auto device = Graphics::GetDevice();
		auto& objectSelected = m_transformableObjects[0];
		auto world = Math::Matrix::CreateScale(objectSelected->Scale) * Math::Matrix::CreateFromQuaternion(objectSelected->Rotation) * Math::Matrix::CreateTranslation(objectSelected->Position);
		objectSelected->WorldTransform = world;

		Math::Matrix mvpMatrix = world * m_camera.GetViewProjection();

		ObjectBuffer objectBuffer;
		objectBuffer.World = mvpMatrix;

		uint32_t frameIndex = device->GetFrameId();
		m_objectBuffer[frameIndex]->SetMappedData(objectBuffer);
	}
}