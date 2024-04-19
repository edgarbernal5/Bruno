#include "brepch.h"
#include "PropertiesPanel.h"

#include "Scene/SelectionService.h"
#include "Scene/SceneDocument.h"
#include "Scene/SceneHierarchy.h"
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
		m_sceneHierarchy = sceneDocument->GetSceneHierarchy();

		m_propertyGrid.create(*this);

		m_place.bind(this->handle());
		////////// VIEW
		m_place.div("vert <properties>");

		m_place["properties"] << m_propertyGrid;
		m_place.collocate();

		m_selectionChangedHandleId = m_sceneDocument->SelectionChanged.connect([&](const std::vector<UUID>& selection)
		{
			BR_CORE_TRACE << "selection changed / selection.size = " << selection.size() << std::endl;

			//TODO: si no hay cambios no refrescar.
			m_propertyGrid.auto_draw(false);
			ClearPropertyGrid();
			m_currentProperties.clear();
			DisposePropertyBinders();

			if (selection.size() != 1)
			{
				m_propertyGrid.auto_draw(true);
				return;
			}

			auto& uuid = selection[0];
			auto& nodeProperties = m_sceneHierarchy->get(uuid);
			
			for (size_t i = 0; i < nodeProperties.size(); i++)
			{
				auto prop = nodeProperties[i];

				auto cat_idx = m_propertyGrid.find(prop.category());
				auto cat = (cat_idx == nana::npos) ? m_propertyGrid.append(prop.category()) : m_propertyGrid.at(cat_idx);
				nana::propertygrid::item_proxy ip(nullptr);

				if (prop.type() == pg_type::string)
				{
					ip = cat.append(nana::propertygrid::pgitem_ptr(new nana::pg_string(prop.label(), prop.value())));
				} else if (prop.type() == pg_type::uint)
				{
					ip = cat.append(nana::propertygrid::pgitem_ptr(new nana::pg_string_uint(prop.label(), prop.value())));
				}
				else if (prop.type() == pg_type::vector3)
				{
					ip = cat.append(nana::propertygrid::pgitem_ptr(new pg_vector3(prop.label(), prop.value())));
				}
				else if (prop.type() == pg_type::asset_file)
				{
					auto pgaf = new pg_asset_file(prop.label(), prop.value());
					ip = cat.append(nana::propertygrid::pgitem_ptr(pgaf));
					pgaf->set_button_click([&](const nana::arg_click& click_args)
					{
						//...
						nana::menu_popuper(m_asset_file_menu_popup, nana::mouse::left_button)(*click_args.mouse_args);
					});
				}
				auto item_ptr = ip._m_pgitem();
				auto handlerId = prop.on_change().connect([item_ptr](const std::string& new_value)
				{
					item_ptr->value(new_value);
				});
				item_ptr->enabled(!prop.read_only());
				m_propOnChangedHandlers[prop] = handlerId;
			}
			m_propertyGrid.auto_draw(true);
		});

		m_asset_file_menu_popup.append("Select asset...", [](nana::menu::item_proxy& ip) {
			//TODO: callback o un objeto. inyectarlo
		});
		m_asset_file_menu_popup.append_splitter();
		m_asset_file_menu_popup.append("Find asset in Content Browser", [](nana::menu::item_proxy& ip) {});

		m_propertyGrid.events().property_changed([this](const nana::arg_propertygrid& arg)
		{
			BR_CORE_TRACE << "property_changed / grid. label = " << arg.item.label() << ". cat = " << arg.item.pos().cat << std::endl;
			
			auto cat = m_propertyGrid.at(arg.item.pos().cat);

			auto& uuid = m_selectionService->GetSelections()[0];
			auto& nodeProperties = m_sceneHierarchy->get(uuid);

			for (size_t i = 0; i < nodeProperties.size(); ++i)
			{
				auto property = nodeProperties[i];
				if (arg.item.label() == property.label() && cat.text() == property.category())
				{
					property.value(arg.item.value());
					break;
				}
			}
		});
	}

	PropertiesPanel::~PropertiesPanel()
	{
		m_sceneDocument->SelectionChanged.disconnect(m_selectionChangedHandleId);
		DisposePropertyBinders();
	}

	void PropertiesPanel::ClearPropertyGrid()
	{
		m_propertyGrid.clear();
		for (size_t i = 0; i < m_propertyGrid.size_categ(); i++)
		{
			m_propertyGrid.erase();
		}
	}

	void PropertiesPanel::DisposePropertyBinders()
	{
		for (auto& [prop, handlerId] : m_propOnChangedHandlers) {
			prop.on_change().disconnect(handlerId);
		}
		m_propOnChangedHandlers.clear();
	}
}