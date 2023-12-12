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
		for (size_t i = 0; i < Graphics::Core::FRAMES_IN_FLIGHT_COUNT; i++)
		{
			m_objectBuffer[i] = std::make_unique<ConstantBuffer<ObjectBuffer>>();
		}
		m_position = Math::Vector3::Zero;
		m_rotation = Math::Quaternion::Identity;
		m_scale = Math::Vector3::One;
	}

	void Scene::AddModel(std::shared_ptr<Model> model)
	{
		auto& meshes = model->GetMeshes();
		for (auto& mesh : meshes)
		{
			auto boxRenderItem = std::make_shared<RenderItem>();
			boxRenderItem->IndexCount = mesh->GetIndexBuffer()->GetElementCount();
			boxRenderItem->IndexBuffer = mesh->GetIndexBuffer();
			boxRenderItem->VertexBuffer = mesh->GetVertexBuffer();
			boxRenderItem->Material = mesh->GetMaterial();
			m_renderItems.push_back(boxRenderItem);
		}

		m_models.push_back(model);
	}

	void Scene::OnUpdate(const GameTimer& timer)
	{
		auto device = Graphics::GetDevice();
		auto world = Math::Matrix::CreateScale(m_scale) * Math::Matrix::CreateFromQuaternion(m_rotation) * Math::Matrix::CreateTranslation(m_position);
		Math::Matrix mvpMatrix = world * m_camera.GetViewProjection();

		ObjectBuffer objectBuffer;
		objectBuffer.World = mvpMatrix;

		uint32_t frameIndex = device->GetFrameId();
		m_objectBuffer[frameIndex]->SetMappedData(objectBuffer);
	}
}