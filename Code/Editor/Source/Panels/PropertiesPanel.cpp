#include "PropertiesPanel.h"

#include "Scene/SelectionService.h"
#include <Bruno/Scene/Scene.h>

#include <nana/gui/widgets/pgitems.hpp>

namespace Bruno
{
	PropertiesPanel::PropertiesPanel(nana::window window, std::shared_ptr<Scene> scene, std::shared_ptr<SelectionService> selectionService) :
		nana::panel<true>(window),
		m_scene(scene),
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
			m_propertyGrid.clear();

			if (selection.size() != 1)
				return;

			bool isMultiSelection = selection.size() > 1;
			auto uuid = selection[0];
			
			auto entity = m_scene->GetEntityWithUUID(uuid);

			auto cat_idx = 0u;//m_propertyGrid.find("name");
			auto cat = (cat_idx == nana::npos) ? m_propertyGrid.append("name") : m_propertyGrid.at(cat_idx);
			nana::propertygrid::item_proxy ip(nullptr);
			ip = cat.append(nana::propertygrid::pgitem_ptr(new nana::pg_string("Name", entity.GetComponent<NameComponent>().Name)));

			//auto& transform = entity.GetComponent<TransformComponent>();
			
		});

		m_propertyGrid.events().property_changed([](const nana::arg_propertygrid& arg)
		{
			BR_CORE_TRACE << "property_changed / grid. label = " << arg.item.label() << ". cat = " << arg.item.pos().cat << std::endl;
		});
	}

	PropertiesPanel::~PropertiesPanel()
	{
		m_selectionService->SelectionChanged.disconnect(m_selectionChangedHandleId);
	}
}