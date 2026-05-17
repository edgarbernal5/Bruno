#include "brepch.h"
#include "PropertiesPanel.h"

#include "Scene/SelectionService.h"
#include "Scene/SceneDocument.h"
#include "Scene/SceneHierarchy.h"
#include <Bruno/Scene/Scene.h>

#include <Berta/Controls/Properties/PropertyGridFields.h>
#include <Berta/GUI/ControlDrawBatch.h>

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
		m_scene = sceneDocument->GetScene();
		
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
			Berta::ControlDrawBatch batchGuard(m_propertyGrid);
			
			m_propertyGrid.Clear();

			if (selection.size() != 1)
			{
				return;
			}

			auto& uuid = selection[0];
			auto entity = m_scene->GetEntityWithUUID(uuid);
			
			if (!entity)
			{
				return;
			}
			
			if (entity.HasComponent<NameComponent>())
			{
				auto categoryGeneral = m_propertyGrid.Append("General");
				
				categoryGeneral.EmplaceProperty<Berta::PropertyGridFieldString>(
					"Name",
					[entity]() -> std::wstring { return entity.GetComponent<NameComponent>().Name; },
					[entity](const std::wstring& val) mutable
					{
						entity.Patch<NameComponent>([&val](auto& component)
						{
						   component.Name = val;
						});
					}
				);
			}
			
			if (entity.HasComponent<TransformComponent>())
			{
				auto categoryTransform = m_propertyGrid.Append("Transform");
				
				categoryTransform.EmplaceVector3("Position", 
				[entity]()
				{
					Berta::OptionalVector3 opt;
					auto& position = entity.GetComponent<TransformComponent>().Position;
					opt.x = position.x;
					opt.y = position.y;
					opt.z = position.z;
					
					return opt;
				},
				[entity](const Berta::OptionalVector3& val) mutable
				{
					auto& position = entity.GetComponent<TransformComponent>().Position;
					if (val.x.has_value())
					{
						position.x = val.x.value();
					}
					
					if (val.y.has_value())
					{
						position.y = val.y.value();
					}
					
					if (val.z.has_value())
					{
						position.z = val.z.value();
					}
				});
				/*
				*auto currentRotation = Math::Quaternion::CreateFromYawPitchRoll(prop.as_vector3());
				currentRotation *= delta;
				prop.value(currentRotation.ToEuler());
				 */
				categoryTransform.EmplaceVector3("Rotation", 
				[entity]()
				{
					Berta::OptionalVector3 opt;
					auto& rotation = entity.GetComponent<TransformComponent>().Rotation;
					auto currentRotation = rotation.ToEuler();
					opt.x = currentRotation.x;
					opt.y = currentRotation.y;
					opt.z = currentRotation.z;
					
					return opt;
				},
				[entity](const Berta::OptionalVector3& val) mutable
				{
					auto& rotation = entity.GetComponent<TransformComponent>().Rotation;
					if (val.x.has_value())
					{
						//Math::Quaternion::CreateFromYawPitchRoll
						rotation.x = val.x.value();
					}
					
					if (val.y.has_value())
					{
						rotation.y = val.y.value();
					}
					
					if (val.z.has_value())
					{
						rotation.z = val.z.value();
					}
				});
				
				categoryTransform.EmplaceVector3("Scale", 
				[entity]()
				{
					Berta::OptionalVector3 opt;
					auto& scale = entity.GetComponent<TransformComponent>().Scale;
					opt.x = scale.x;
					opt.y = scale.y;
					opt.z = scale.z;
					
					return opt;
				},
				[entity](const Berta::OptionalVector3& val) mutable
				{
					auto& scale = entity.GetComponent<TransformComponent>().Scale;
					if (val.x.has_value())
					{
						scale.x = val.x.value();
					}
					
					if (val.y.has_value())
					{
						scale.y = val.y.value();
					}
					
					if (val.z.has_value())
					{
						scale.z = val.z.value();
					}
				});
			}
			
			//TODO
			/*for (size_t i = 0; i < nodeProperties.size(); i++)
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
			}*/
		});

		m_asset_file_menu_popup.Append("Select asset...", [](Berta::MenuItem ip)
		{
			//TODO: callback o un objeto. inyectarlo
		});
		m_asset_file_menu_popup.AppendSeparator();
		m_asset_file_menu_popup.Append("Find asset in Content Browser", [](Berta::MenuItem ip) {});

		m_propertyGrid.GetEvents().PropertyChanged.Connect([this](const Berta::ArgPropertyGrid& args)
		{
			BR_CORE_TRACE << "property_changed / grid. label = " << args.Property.GetLabel() << ". value = " << Berta::StringUtils::WideToUTF8(args.Property.GetValueAsString()) << std::endl;
		});
	}

	PropertiesPanel::~PropertiesPanel()
	{
		m_sceneDocument->SelectionChanged.disconnect(m_selectionChangedHandleId);
	}
}