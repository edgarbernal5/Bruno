#include "PropertiesPanel.h"

#include "Scene/SelectionService.h"
#include "Scene/SceneDocument.h"
#include <Bruno/Scene/Scene.h>

#include <nana/gui/widgets/pgitems.hpp>
#include "Properties/PropertyGridItems.h"

namespace Bruno
{
	PropertiesPanel::PropertiesPanel(nana::window window, std::shared_ptr<SceneDocument> sceneDocument) :
		nana::panel<true>(window),
		m_sceneDocument(sceneDocument)
	{
		this->caption("Properties");

		m_selectionService = sceneDocument->GetSelectionService();

		m_propertyGrid.create(*this);

		m_place.bind(this->handle());
		////////// VIEW
		m_place.div("vert <properties>");

		m_place["properties"] << m_propertyGrid;
		m_place.collocate();

		m_selectionChangedHandleId = m_sceneDocument->SelectionChanged.connect([&](const std::vector<UUID>& selection)
		{
			m_propertyGrid.clear();
			m_properties.clear();
			m_propToCallbacks.clear();

			if (selection.size() != 1)
				return;

			bool isMultiSelection = selection.size() > 1;
			auto& uuid = selection[0];
			
			auto entity = m_sceneDocument->GetScene()->GetEntityWithUUID(uuid);

			{
				auto& name = entity.GetComponent<NameComponent>().Name;
				auto prop = m_properties.append("Name");
				prop.value(name);
				prop.label("Name");
				prop.category("");
				m_propToCallbacks[prop] = [](Entity entity, const std::string& new_value)
				{
					entity.GetComponent<NameComponent>().Name = new_value;
				};

				auto cat_idx = 0u;//m_propertyGrid.find(prop.category());
				auto cat = (cat_idx == nana::npos) ? m_propertyGrid.append(prop.category()) : m_propertyGrid.at(cat_idx);
				nana::propertygrid::item_proxy ip(nullptr);
				ip = cat.append(nana::propertygrid::pgitem_ptr(new nana::pg_string(prop.label(), prop.value())));
			}
			{
				auto& transform = entity.GetComponent<TransformComponent>();
				auto prop = m_properties.append("Transform/Position");
				prop.value(transform.Position);
				prop.label("Position");
				prop.category("Transform");
				m_propToCallbacks[prop] = [prop](Entity entity, const std::string& new_value)
				{
					entity.GetComponent<TransformComponent>().Position = prop.as_vector3();
				};
				
				auto cat_idx = m_propertyGrid.find(prop.category());
				auto cat = (cat_idx == nana::npos) ? m_propertyGrid.append(prop.category()) : m_propertyGrid.at(cat_idx);
				nana::propertygrid::item_proxy ip(nullptr);
				ip = cat.append(nana::propertygrid::pgitem_ptr(new pg_vector3(prop.label(), prop.value())));
			}
		});

		m_propertyGrid.events().property_changed([this](const nana::arg_propertygrid& arg)
		{
			BR_CORE_TRACE << "property_changed / grid. label = " << arg.item.label() << ". cat = " << arg.item.pos().cat << std::endl;
			
			auto cat = m_propertyGrid.at(arg.item.pos().cat);
			for (size_t i = 0; i < m_properties.size(); ++i)
			{
				auto property = m_properties[i];
				if (arg.item.label() == property.label() && cat.text() == property.category())
				{
					property.value(arg.item.value());
					auto uuid = m_selectionService->GetSelections()[0];
					auto entity = m_sceneDocument->GetScene()->GetEntityWithUUID(uuid);
					m_propToCallbacks[property](entity, arg.item.value());
					break;
				}
			}
		});
	}

	PropertiesPanel::~PropertiesPanel()
	{
		m_sceneDocument->SelectionChanged.disconnect(m_selectionChangedHandleId);
	}
}