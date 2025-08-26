#pragma once

#include <Berta/Controls/Panel.h>
#include <Berta/GUI/Layout.h>
#include <Berta/Controls/Menu.h>
#include <Berta/Controls/PropertyGrid.h>
#include "Properties/Property.h"

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
		~PropertiesPanel();

	private:
		void ClearPropertyGrid();
		void DisposePropertyBinders();

		std::shared_ptr<SceneDocument> m_sceneDocument;
		std::shared_ptr<SceneHierarchy> m_sceneHierarchy;
		std::shared_ptr<SelectionService> m_selectionService;
		EventHandlerId m_selectionChangedHandleId{ 0 };

		Berta::Layout m_layout;
		Berta::PropertyGrid m_propertyGrid;
		properties_collection m_currentProperties;
		std::unordered_map<property_proxy, size_t> m_propOnChangedHandlers;
		Berta::Menu m_asset_file_menu_popup;
	};
}