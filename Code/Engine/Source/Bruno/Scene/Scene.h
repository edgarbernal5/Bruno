#pragma once

#include <vector>
#include "Bruno/Platform/DirectX/ConstantBuffer.h"
#include <Bruno/Renderer/Camera.h>

namespace Bruno
{
	class Camera;
	class GameTimer;
	class Model;
	struct RenderItem;

	class Scene
	{
	public:
		Scene(Camera& camera);
		void AddModel(std::shared_ptr<Model> model);
		const std::vector<std::shared_ptr<RenderItem>>& GetRenderItems() { return m_renderItems; }
		void OnUpdate(const GameTimer& timer);

		friend class SceneRenderer;

	private:
		Camera& m_camera;
		std::vector<std::shared_ptr<Model>>			m_models;
		std::vector<std::shared_ptr<RenderItem>>	m_renderItems;

		struct ObjectBuffer
		{
			Math::Matrix World;
		};
		std::unique_ptr<ConstantBuffer<ObjectBuffer>> m_objectBuffer[Graphics::Core::FRAMES_IN_FLIGHT_COUNT];
	};
}

