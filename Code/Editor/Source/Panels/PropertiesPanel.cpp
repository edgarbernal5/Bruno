#include "brepch.h"
#include "PropertiesPanel.h"

#include "Scene/SelectionService.h"
#include "Scene/SceneDocument.h"
#include "Scene/SceneHierarchy.h"
#include <Bruno/Scene/Scene.h>

#include <Berta/Controls/Properties/PropertyGridFields.h>
#include <Berta/GUI/ControlDrawBatch.h>

#include "Content/EditorAssetManager.h"
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
				
				categoryTransform.EmplaceVector3(
					"Position", 
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
					}
				);
				/*
				*auto currentRotation = Math::Quaternion::CreateFromYawPitchRoll(prop.as_vector3());
				currentRotation *= delta;
				prop.value(currentRotation.ToEuler());
				 */
				categoryTransform.EmplaceVector3(
					"Rotation", 
				[entity]()
					{
						Berta::OptionalVector3 opt;
						auto& rotation = entity.GetComponent<TransformComponent>().Rotation;
						//auto currentRotation = rotation.ToEuler();
						opt.x = rotation.x;
						opt.y = rotation.y;
						opt.z = rotation.z;
						
						return opt;
					},
					[entity](const Berta::OptionalVector3& val) mutable
					{
						auto& rotation = entity.GetComponent<TransformComponent>().Rotation;
						
						if (val.x.has_value())
						{
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
					}
				);
				
				categoryTransform.EmplaceVector3(
					"Scale", 
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
					}
				);
			}
			
			if (entity.HasComponent<ModelComponent>())
			{
				auto modelCategory = m_propertyGrid.Append("Model");
				modelCategory.EmplaceProperty<Berta::PropertyGridFieldString>(
					"Handle", 
					[entity]() -> std::wstring
					{
						auto handleToStr = std::to_wstring(entity.GetComponent<ModelComponent>().ModelHandle);
						return handleToStr;
					}, nullptr).SetReadOnly(true);

				modelCategory.EmplaceProperty<Berta::PropertyGridFieldUInt>("Mesh index", 
				[entity]() -> uint32_t
				{
					return entity.GetComponent<ModelComponent>().MeshIndex;
				}, nullptr).SetReadOnly(true);
				
				auto& assetManager = *m_sceneDocument->GetAssetManager();
				auto modelMaterialsSubCategories = modelCategory.AppendSubCategory("Materials");
				auto& modelComp = entity.GetComponent<ModelComponent>();
				auto ownerWindow = this->Handle();
				for (auto& [index, materialAssetHandle] : modelComp.Materials->GetMaterials())
				{
					std::ostringstream oss;
					oss << "Material " << index;
					
					modelMaterialsSubCategories.EmplaceProperty<Berta::PropertyGridFieldStringButton>(oss.str(), 
						[entity, index, &assetManager]() -> std::wstring
						{
							UUID currentHandle = entity.GetComponent<ModelComponent>().Materials->GetMaterial(index);
							if (currentHandle == static_cast<UUID>(0))
							{
								return L"None";
							}
							if (auto metadata = assetManager.GetMetadata(currentHandle))
							{
								return metadata.Filename;
							}
							
							return L"";
						},
						[entity, index, &assetManager](const std::wstring& typedPath) mutable
						{
							UUID newHandle = 0; //assetManager.LoadOrCreateAsset(typedPath);
							entity.Patch<ModelComponent>([newHandle, index](auto& comp)
							{
								comp.Materials->SetMaterial(index, newHandle);
							});
						},
						[entity, index, materialAssetHandle, ownerWindow, &assetManager](std::optional<std::wstring> currentValue)
						{
							Berta::Menu menuContext;
							
							menuContext.Append("Clear Material", [entity, index](Berta::MenuItem ip)
							{
								std::cout << "Clear Material " << index << std::endl;
								entity.Patch<ModelComponent>([index](auto& comp)
								{
									comp.Materials->SetMaterial(index, 0);
								});
							});
							menuContext.Append("Select asset...", [entity, materialAssetHandle](Berta::MenuItem ip)
							{
								
							});
							menuContext.AppendSeparator();
							
							menuContext.Append("Find asset in Content Browser", [entity, index](Berta::MenuItem ip)
							{
								
							});
							Berta::GUI::ShowContextMenu(std::move(menuContext), ownerWindow, Berta::GUI::GetMousePositionToWindow(ownerWindow));
						});
				}
			}
		});
		
		m_propertyGrid.GetEvents().PropertyChanged.Connect([this](const Berta::ArgPropertyGrid& args)
		{
			BR_CORE_TRACE << "property_changed / grid. label = " << args.Property.GetLabel() << ". value = " << Berta::StringUtils::WideToUTF8(args.Property.GetValueAsString()) << std::endl;
		});
		
		m_nameUpdateConnection = m_sceneDocument->GetScene()->OnComponentUpdated<NameComponent>().connect<&PropertiesPanel::OnComponentUpdated>(this);
		m_transformUpdateConnection = m_sceneDocument->GetScene()->OnComponentUpdated<TransformComponent>().connect<&PropertiesPanel::OnComponentUpdated>(this);
		m_modelUpdateConnection = m_sceneDocument->GetScene()->OnComponentUpdated<ModelComponent>().connect<&PropertiesPanel::OnComponentUpdated>(this);
	}

	PropertiesPanel::~PropertiesPanel()
	{
		m_sceneDocument->SelectionChanged.disconnect(m_selectionChangedHandleId);
	}

	void PropertiesPanel::OnComponentUpdated(entt::registry& registry, entt::entity updatedEntity)
	{
		auto selection = m_selectionService->GetSelections();
		if (selection.size() != 1)
		{
			return;
		}

		auto selectedEntity = m_scene->GetEntityWithUUID(selection[0]);
		Entity entity{ updatedEntity, m_sceneDocument->GetScene().get() };
		
		if (selectedEntity && selectedEntity == entity)
		{
			m_propertyGrid.RefreshAll();
		}
	}
}
