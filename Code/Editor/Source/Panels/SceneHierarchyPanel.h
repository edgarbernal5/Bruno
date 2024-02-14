#pragma once

#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/treebox.hpp>
#include "Bruno/Core/UUID.h"

namespace Bruno
{
	class Scene;
	class Entity;
	class SelectionService;
	class GizmoService;
	enum class ActionMode;

	class SceneHierarchyPanel : public nana::panel<true>
	{
	public:
		SceneHierarchyPanel(nana::window window, std::shared_ptr<Scene> scene, std::shared_ptr<SelectionService> selectionService, std::shared_ptr<GizmoService> gizmoService);

		void DeselectAll();
		void Select(UUID selectionUUID);

		friend class ScenePanel;
	private:
		void OnHierarchyAdded(Entity& entity, const std::string& parentKey="");

		nana::place m_place;
		nana::treebox m_treebox;
		std::shared_ptr<Scene> m_scene;
		std::shared_ptr<SelectionService> m_selectionService;
		std::shared_ptr<GizmoService> m_gizmoService;

		std::unordered_map<UUID, nana::treebox::item_proxy> m_entityToNodeMap;
	};
}
