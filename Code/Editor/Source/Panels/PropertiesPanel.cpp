#include "brepch.h"
#include "PropertiesPanel.h"

#include "Scene/SelectionService.h"
#include "Scene/SceneDocument.h"
#include "Scene/SceneHierarchy.h"
#include <Bruno/Scene/Scene.h>

#include <Berta/Controls/Properties/PropertyGridFields.h>
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
			for (size_t i = 0; i < nodeProperties.size(); i++)
			{
				auto prop = nodeProperties[i];

				auto cat = m_propertyGrid.Find(prop.GetCategory());
				if (!cat)
				{
					cat = m_propertyGrid.Append(prop.GetCategory());
				}
				Berta::PropertyGrid::PropertyItem pi;

				if (prop.GetType() == PropertyGridType::String)
				{
					pi = cat.Append(Berta::PropertyGrid::PropertyGridFieldBasePtr(new Berta::PropertyGridFieldString(prop.GetLabel(), prop.GetValue())));
				} 
				else if (prop.GetType() == PropertyGridType::Uint)
				{
					pi = cat.Append(Berta::PropertyGrid::PropertyGridFieldBasePtr(new Berta::PropertyGridFieldStringUInt(prop.GetLabel(), prop.GetValue())));
				}
				else if (prop.GetType() == PropertyGridType::Vector3)
				{
					pi = cat.Append(Berta::PropertyGrid::PropertyGridFieldBasePtr(new Berta::PropertyGridFieldVector3(prop.GetLabel(), prop.GetValue())));
				}
				else if (prop.GetType() == PropertyGridType::AssetFile)
				{
					//auto pgaf = new pg_asset_file(prop.label(), prop.value());
					//pi = cat.Append(Berta::PropertyGrid::pgitem_ptr(pgaf));
					//pgaf->set_button_click([&](const Berta::arg_click& click_args)
					//{
					//	//...
					//	Berta::menu_popuper(m_asset_file_menu_popup, Berta::mouse::left_button)(*click_args.mouse_args);
					//});
				}

				if (!pi)
				{
					continue;
				}
				
				auto item_ptr = pi.GetPropertyFieldPtr();
				const auto handlerId = prop.on_change().connect([item_ptr](const std::string& newValue)
					{
						item_ptr->SetValue(newValue);
					});
				
				pi.SetEnabled(!prop.IsReadOnly());
				m_propOnChangedHandlers[prop] = handlerId;
			}
			m_propertyGrid.SetAutoDraw(true);
		});

		m_asset_file_menu_popup.Append("Select asset...", [](Berta::MenuItem& ip) {
			//TODO: callback o un objeto. inyectarlo
		});
		m_asset_file_menu_popup.AppendSeparator();
		m_asset_file_menu_popup.Append("Find asset in Content Browser", [](Berta::MenuItem& ip) {});

		m_propertyGrid.GetEvents().PropertyChanged.Connect([this](const Berta::ArgPropertyGrid& args)
		{
			BR_CORE_TRACE << "property_changed / grid. label = " << args.Property.GetLabel() << ". value = " << args.Property.GetValue() << std::endl;
			
			//TODO
			//auto cat = m_propertyGrid.at(arg.item.pos().cat);
			
			auto& uuid = m_selectionService->GetSelections()[0];
			auto& nodeProperties = m_sceneHierarchy->get(uuid);

			for (size_t i = 0; i < nodeProperties.size(); ++i)
			{
				auto property = nodeProperties[i];
				if (args.Property.GetLabel() == property.GetLabel() /*&& cat.text() == property.category()*/)
				{
					property.SetValue(args.Property.GetValue());
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