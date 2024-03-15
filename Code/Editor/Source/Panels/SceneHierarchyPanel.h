#pragma once

#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/treebox.hpp>
#include "Bruno/Core/UUID.h"

namespace Bruno
{
	class SceneDocument;
	class Entity;
	class SelectionService;
	class GizmoService;
	enum class ActionMode;

	class SceneHierarchyPanel : public nana::panel<true>
	{
	public:
		SceneHierarchyPanel(nana::window window, std::shared_ptr<SceneDocument> sceneDocument);
		~SceneHierarchyPanel();

		friend class ScenePanel;
	private:
		void OnHierarchyAdded(Entity entity, const std::string& parentKey="");

		nana::place m_place;
		nana::treebox m_treebox;
		std::shared_ptr<SceneDocument> m_sceneDocument;
		std::shared_ptr<SelectionService> m_selectionService;
		std::shared_ptr<GizmoService> m_gizmoService;

		bool m_ignoreEvents{ false };
		std::unordered_map<UUID, nana::treebox::item_proxy> m_entityToNodeMap;
		size_t m_selectionChangedHandleId{ 0 };
		size_t m_hierarchyChangedHandleId{ 0 };
	};
}
