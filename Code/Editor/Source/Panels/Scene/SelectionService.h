#pragma once

#include <vector>
#include <Bruno/Core/UUID.h>
#include <Bruno/Core/Events/Event.h>
#include <Bruno/Math/Math.h>
#include <Bruno/Renderer/Camera.h>
#include <entt/entt.hpp>

namespace Bruno
{
	class Entity;
	struct BoundingBoxComponent;
	class Scene;
	class EditorAssetManager;

	class SelectionService
	{
	public:
		SelectionService(std::shared_ptr<Scene> scene, EditorAssetManager* assetManager);
		~SelectionService();
		
		void SelectUnderMousePosition(const Camera& camera, const Math::Int2& mousePosition);
		void DeselectAll();
		size_t GetSelectionCount() const { return m_selections.size(); }
		bool GetGizmoTransform(Math::Matrix& outMatrix, Math::Vector3& outPosition);
		
		const std::vector<UUID>& GetSelections() const { return m_selections; }
		void Select(UUID selection);
		void Select(const std::vector<UUID>& selection);
		void Deselect(UUID selection);
		
		void SelectEntitiesInRect(const Camera& camera, const Math::Vector2& ndcRectMin, const Math::Vector2& ndcRectMax);

		// Métodos para controlar el batching
		void BeginBatch();
		void EndBatch();
		
		Event<std::vector<UUID>> SelectionChanged;
	private:
		void OnEntitySelected(entt::registry& registry, entt::entity entityHandle);
		void OnEntityDeselected(entt::registry& registry, entt::entity entityHandle);
		
		Math::Ray ConvertMousePositionToRay(Camera camera, const Math::Int2& mousePosition);
		UUID FindEntityUUIDWithRay(const Math::Ray& ray, float maxDistance, Entity& outEntity);
		
		bool ProjectAABBToNDC(const BoundingBoxComponent& bbox, const Math::Matrix& wvp, Math::Vector2& outMin, Math::Vector2& outMax, bool& outIsBehindCamera);
		bool CheckRectIntersection(const Math::Vector2& rectA_Min, const Math::Vector2& rectA_Max, const Math::Vector2& rectB_Min, const Math::Vector2& rectB_Max);
		
		std::shared_ptr<Scene> m_scene;
		EditorAssetManager* m_assetManager;
		
		bool m_isBatching = false;
		bool m_isDirty = false; // Nos dice si debemos emitir al terminar el batch
		std::vector<UUID> m_selections;
	};
	
	struct ScopedSelectionBatch
	{
		SelectionService& Service;

		ScopedSelectionBatch(SelectionService& service) : Service(service) 
		{ 
			Service.BeginBatch(); 
		}

		~ScopedSelectionBatch() 
		{ 
			Service.EndBatch(); 
		}
	};
}

