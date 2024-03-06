#include "PropertiesPanel.h"

#include "Scene/SelectionService.h"

namespace Bruno
{
	PropertiesPanel::PropertiesPanel(nana::window window, std::shared_ptr<SelectionService> selectionService) :
		nana::panel<true>(window),
		m_selectionService(selectionService)
	{
		this->caption("Properties");

		m_propertyGrid.create(*this);

		m_place.bind(this->handle());
		////////// VIEW
		m_place.div("vert <properties>");

		m_place["properties"] << m_propertyGrid;
		m_place.collocate();

		m_selectionChangedHandleId = m_selectionService->SelectionChanged.connect([&](const std::vector<UUID>& selection)
		{
		});
	}

	PropertiesPanel::~PropertiesPanel()
	{
		m_selectionService->SelectionChanged.disconnect(m_selectionChangedHandleId);
	}
}