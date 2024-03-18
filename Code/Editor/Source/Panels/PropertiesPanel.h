#pragma once

#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/property_grid.hpp>
#include "Properties/Property.h"

namespace Bruno
{
	class SelectionService;
	class SceneHierarchy;
	class SceneDocument;
	class Entity;

	class PropertiesPanel : public nana::panel<true>
	{
	public:
		PropertiesPanel(nana::window window, std::shared_ptr<SceneDocument> sceneDocument);
		~PropertiesPanel();

	private:
		void DisposePropertyBinders();

		std::shared_ptr<SceneDocument> m_sceneDocument;
		std::shared_ptr<SceneHierarchy> m_sceneHierarchy;
		std::shared_ptr<SelectionService> m_selectionService;
		EventHandlerId m_selectionChangedHandleId{ 0 };

		nana::place m_place;
		nana::propertygrid m_propertyGrid;
		properties_collection m_currentProperties;
		std::unordered_map<property_proxy, size_t> m_propOnChangedHandlers;
	};
}