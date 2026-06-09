#pragma once

#include <Berta/Controls/Panel.h>
#include <Berta/GUI/Layout.h>
#include <Berta/Controls/Menu.h>
#include <Berta/Controls/PropertyGrid.h>

#include "Bruno/Core/Events/Event.h"
#include "Bruno/Scene/Scene.h"

namespace Bruno
{
	class SelectionService;
	class SceneHierarchy;
	class SceneDocument;
	class Entity;

	class PropertiesPanel : public Berta::Panel
	{
	public:
		PropertiesPanel(Berta::Window* window, std::shared_ptr<SceneDocument> sceneDocument);
		~PropertiesPanel() override;

	private:
		void OnComponentUpdated(entt::registry& registry, entt::entity updatedEntity);

		std::shared_ptr<SceneDocument> m_sceneDocument;
		std::shared_ptr<SceneHierarchy> m_sceneHierarchy;
		std::shared_ptr<Scene> m_scene;
		std::shared_ptr<SelectionService> m_selectionService;
		EventHandlerId m_selectionChangedHandleId{ 0 };

		Berta::Layout m_layout;
		Berta::PropertyGrid m_propertyGrid;
		entt::scoped_connection m_nameUpdateConnection;
		entt::scoped_connection m_transformUpdateConnection;
		entt::scoped_connection m_modelUpdateConnection;
	};
}
