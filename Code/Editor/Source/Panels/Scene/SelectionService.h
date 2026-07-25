#pragma once

#include <vector>
#include <Bruno/Core/UUID.h>
#include <Bruno/Core/Events/Event.h>
#include <Bruno/Math/Math.h>
#include <Bruno/Renderer/Camera.h>

namespace Bruno
{
	struct BoundingBoxComponent;
	class Scene;
	class EditorAssetManager;

	class SelectionService
	{
	public:
		SelectionService(std::shared_ptr<Scene> scene, EditorAssetManager* assetManager);

		void SelectUnderMousePosition(const Camera& camera, const Math::Int2& mousePosition);
		void DeselectAll();
		Math::Matrix GetSelectionLocalTransform();
		Math::Matrix GetSelectionTransform();
		size_t GetSelectionCount() { return m_selections.size(); }

		const std::vector<UUID>& GetSelections() const { return m_selections; }
		void Select(UUID selection);
		void Deselect(UUID selection);
		
		void SelectEntitiesInRect(const Math::Matrix& viewProjection,
									 const Math::Vector2& ndcRectMin, 
									 const Math::Vector2& ndcRectMax);

		Event<std::vector<UUID>> SelectionChanged;
	private:
		Math::Ray ConvertMousePositionToRay(Camera camera, const Math::Int2& mousePosition);
		UUID FindEntityUUIDWithRay(const Math::Ray& ray, float maxDistance);
		
		bool ProjectAABBToNDC(const BoundingBoxComponent& bbox, 
								 const Math::Matrix& wvp, 
								 Math::Vector2& outMin, 
								 Math::Vector2& outMax, 
								 bool& outIsBehindCamera);
		
		bool CheckRectIntersection(const Math::Vector2& rectA_Min, const Math::Vector2& rectA_Max, const Math::Vector2& rectB_Min, const Math::Vector2& rectB_Max);
		std::shared_ptr<Scene> m_scene;
		EditorAssetManager* m_assetManager;

		std::vector<UUID> m_selections;
	};
}

