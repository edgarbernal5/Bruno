#pragma once

#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/property_grid.hpp>

namespace Bruno
{
	class SelectionService;
	class Scene;

	class PropertiesPanel : public nana::panel<true>
	{
	public:
		PropertiesPanel(nana::window window, std::shared_ptr<Scene> scene, std::shared_ptr<SelectionService> selectionService);
		~PropertiesPanel();

	private:
		std::shared_ptr<Scene> m_scene;
		std::shared_ptr<SelectionService> m_selectionService;
		size_t m_selectionChangedHandleId{ 0 };

		nana::place m_place;
		nana::propertygrid m_propertyGrid;
	};
}