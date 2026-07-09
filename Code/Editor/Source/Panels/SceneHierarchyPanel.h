#pragma once

#include <Berta/Controls/Panel.h>
#include <Berta/GUI/Layout.h>
#include <Berta/Controls/TreeBox.h>
#include <Bruno/Core/UUID.h>
#include <Bruno/Core/Events/Event.h>
#include <entt/entt.hpp>

namespace Bruno
{
	class SceneDocument;
	class SceneHierarchy;
	class Entity;
	class SelectionService;
	class GizmoService;
	enum class ActionMode;

	class SceneHierarchyPanel : public Berta::Panel
	{
	public:
		SceneHierarchyPanel(Berta::Window* window, std::shared_ptr<SceneDocument> sceneDocument);
		~SceneHierarchyPanel() override;

		friend class ScenePanel;
	private:
		void OnHierarchyAdded(Entity entity, const std::wstring& parentKey = L"");
		void OnEntityNameUpdated(entt::registry& registry, entt::entity entityHandle);
		
		Berta::Layout m_layout;
		Berta::TreeBox m_treebox;
		std::shared_ptr<SceneDocument> m_sceneDocument;
		std::shared_ptr<SceneHierarchy> m_sceneHierarchy;
		std::shared_ptr<SelectionService> m_selectionService;

		bool m_ignoreEvents{ false };
		std::unordered_map<UUID, Berta::TreeBoxItem> m_entityToNodeMap;
		EventHandlerId m_selectionChangedHandleId{ 0 };
		EventHandlerId m_hierarchyChangedHandleId{ 0 };
	};
}
