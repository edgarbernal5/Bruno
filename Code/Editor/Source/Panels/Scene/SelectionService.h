#pragma once

#include <vector>
#include <Bruno/Core/UUID.h>
#include <Bruno/Core/Events/Event.h>
#include <Bruno/Math/Math.h>
#include <Bruno/Renderer/Camera.h>

namespace Bruno
{
	class Scene;
	class AbstractAssetManager;

	class SelectionService
	{
	public:
		SelectionService(std::shared_ptr<Scene> scene, AbstractAssetManager* assetManager);

		void SelectUnderMousePosition(const Camera& camera, const Math::Int2& mousePosition);
		void DeselectAll();
		Math::Matrix GetSelectionLocalTransform();
		Math::Matrix GetSelectionTransform();
		size_t GetSelectionCount() { return m_selections.size(); }

		const std::vector<UUID>& GetSelections() { return m_selections; }
		void Select(UUID selection);

		Event<std::vector<UUID>> SelectionChanged;
	private:
		Math::Ray ConvertMousePositionToRay(Camera camera, const Math::Int2& mousePosition);
		UUID FindEntityUUIDWithRay(const Math::Ray& ray, float maxDistance);

		std::shared_ptr<Scene> m_scene;
		AbstractAssetManager* m_assetManager;

		std::vector<UUID> m_selections;
	};
}

