#pragma once

#include <vector>
#include <Bruno/Core/UUID.h>
#include <Bruno/Math/Math.h>

namespace Bruno
{
	class Scene;
	class AbstractAssetManager;

	class SelectionService
	{
	public:
		SelectionService(std::shared_ptr<Scene> scene, AbstractAssetManager* assetManager);

		void Select(UUID selection);
		UUID FindEntityWithRay(Math::Ray ray, float maxDistance);
		Math::Matrix GetSelectionTransform();

		const std::vector<UUID>& GetSelections() { return m_selections; }
	private:
		std::shared_ptr<Scene> m_scene;
		AbstractAssetManager* m_assetManager;

		std::vector<UUID> m_selections;
	};
}

