#include "brepch.h"
#include "PropertiesPanel.h"

#include "Scene/SelectionService.h"
#include "Scene/SceneDocument.h"
#include "Scene/SceneHierarchy.h"
#include <Bruno/Scene/Scene.h>

#include "Properties/PropertyGridItems.h"

namespace Bruno
{
	PropertiesPanel::PropertiesPanel(Berta::Window* window, std::shared_ptr<SceneDocument> sceneDocument) :
		Berta::Panel(window),
		m_sceneDocument(sceneDocument)
	{
		this->SetCaption("Properties");

		m_selectionService = sceneDocument->GetSelectionService();
		m_sceneHierarchy = sceneDocument->GetSceneHierarchy();

		m_propertyGrid.Create(*this);

		m_layout.Create(this->Handle());
		////////// VIEW
		m_layout.Parse("{VerticalLayout {properties}}");

		m_layout.Attach("properties", m_propertyGrid);
		m_layout.Apply();

		m_selectionChangedHandleId = m_sceneDocument->SelectionChanged.connect([&](const std::vector<UUID>& selection)
		{
			BR_CORE_TRACE << "selection changed / selection.size = " << selection.size() << std::endl;

			//TODO: si no hay cambios no refrescar.
			m_propertyGrid.SetAutoDraw(false);
			ClearPropertyGrid();
			m_currentProperties.clear();
			DisposePropertyBinders();

			if (selection.size() != 1)
			{
				m_propertyGrid.SetAutoDraw(true);
				return;
			}

			auto& uuid = selection[0];
			auto& nodeProperties = m_sceneHierarchy->get(uuid);
			
			//TODO
			//for (size_t i = 0; i < nodeProperties.size(); i++)
			//{
			//	auto prop = nodeProperties[i];

			//	auto cat_idx = m_propertyGrid.find(prop.category());
			//	auto cat = (cat_idx == Berta::npos) ? m_propertyGrid.append(prop.category()) : m_propertyGrid.at(cat_idx);
			//	Berta::PropertyGrid::item_proxy ip(nullptr);

			//	if (prop.type() == pg_type::string)
			//	{
			//		ip = cat.append(Berta::propertygrid::pgitem_ptr(new Berta::pg_string(prop.label(), prop.value())));
			//	} 
			//	else if (prop.type() == pg_type::uint)
			//	{
			//		ip = cat.append(Berta::propertygrid::pgitem_ptr(new Berta::pg_string_uint(prop.label(), prop.value())));
			//	}
			//	else if (prop.type() == pg_type::vector3)
			//	{
			//		ip = cat.append(Berta::propertygrid::pgitem_ptr(new pg_vector3(prop.label(), prop.value())));
			//	}
			//	else if (prop.type() == pg_type::asset_file)
			//	{
			//		auto pgaf = new pg_asset_file(prop.label(), prop.value());
			//		ip = cat.append(Berta::propertygrid::pgitem_ptr(pgaf));
			//		pgaf->set_button_click([&](const Berta::arg_click& click_args)
			//		{
			//			//...
			//			Berta::menu_popuper(m_asset_file_menu_popup, Berta::mouse::left_button)(*click_args.mouse_args);
			//		});
			//	}
			//	auto item_ptr = ip._m_pgitem();
			//	auto handlerId = prop.on_change().connect([item_ptr](const std::string& new_value)
			//	{
			//		item_ptr->value(new_value);
			//	});
			//	item_ptr->enabled(!prop.read_only());
			//	m_propOnChangedHandlers[prop] = handlerId;
			//}
			m_propertyGrid.SetAutoDraw(true);
		});

		m_asset_file_menu_popup.Append("Select asset...", [](Berta::MenuItem& ip) {
			//TODO: callback o un objeto. inyectarlo
		});
		m_asset_file_menu_popup.AppendSeparator();
		m_asset_file_menu_popup.Append("Find asset in Content Browser", [](Berta::MenuItem& ip) {});

		m_propertyGrid.GetEvents().PropertyChanged.Connect([this](const Berta::ArgPropertyGrid& arg)
		{
			BR_CORE_TRACE << "property_changed / grid. label = " << arg.Property.GetLabel() << ". value = " << arg.Property.GetValue() << std::endl;
			
			//TODO
			/*auto cat = m_propertyGrid.at(arg.item.pos().cat);

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
			}*/
		});
	}

	PropertiesPanel::~PropertiesPanel()
	{
		m_sceneDocument->SelectionChanged.disconnect(m_selectionChangedHandleId);
		DisposePropertyBinders();
	}

	void PropertiesPanel::ClearPropertyGrid()
	{
		m_propertyGrid.Clear();
	}

	void PropertiesPanel::DisposePropertyBinders()
	{
		for (auto& [prop, handlerId] : m_propOnChangedHandlers) {
			prop.on_change().disconnect(handlerId);
		}
		m_propOnChangedHandlers.clear();
	}
}