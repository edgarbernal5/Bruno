#include "brepch.h"
#include "SceneHierarchyPanel.h"

#include <Bruno/Scene/Scene.h>
#include <Bruno/Scene/Entity.h>
#include "Scene/SceneDocument.h"
#include "Scene/SelectionService.h"

namespace Bruno
{
	SceneHierarchyPanel::SceneHierarchyPanel(Berta::Window* window, std::shared_ptr<SceneDocument> sceneDocument) :
		Berta::Panel(window),
		m_sceneDocument(sceneDocument)
	{
		this->SetCaption("Hierarchy");

		m_selectionService = m_sceneDocument->GetSelectionService();

		m_treebox.Create(*this);
		
		m_layout.Create(*this);
		m_layout.Parse("{VerticalLayout {tree}}");

		m_layout.Attach("tree", m_treebox);
		m_layout.Apply();
		
		m_treebox.GetEvents().Selected.Connect([&](const Berta::ArgTreeBoxSelection& args)
		{
			//BR_CORE_TRACE << "tree item selected: " << args.item.text() << ". " << args.operated << ". ignore events " << m_ignoreEvents << std::endl;

			if (m_ignoreEvents || args.Items.empty())
				return;

			std::vector<UUID> uuids;
			uuids.reserve(args.Items.size());
			for (auto& item : args.Items)
			{
				uuids.push_back(item.GetUserData<UUID>());
			}
			m_selectionService->DeselectAll();
			m_selectionService->Select(uuids);
		});

		m_hierarchyChangedHandleId = sceneDocument->HierarchyChanged.connect([&](Entity entity, ActionMode actionMode)
		{
			switch (actionMode)
			{
			case Bruno::ActionMode::Add:
				OnHierarchyAdded(entity);
				break;
			case Bruno::ActionMode::Delete:
				break;
			case Bruno::ActionMode::Modify:
				break;
			default:
				break;
			}
		});

		m_treebox.EnableMultiselection(true);

		m_selectionChangedHandleId = m_selectionService->SelectionChanged.connect([&](const std::vector<UUID>& selection)
		{
			m_ignoreEvents = true;
			std::vector<Berta::TreeBoxItem> items;
			items.reserve(selection.size());
			for (auto& item : selection)
			{
				items.push_back(m_entityToNodeMap[item]);
			}
			m_treebox.SelectItems(items);
			
			m_ignoreEvents = false;
		});

		auto scene = sceneDocument->GetScene().get();
		auto entities = scene->GetAllEntitiesWith<IdComponent>();
		for (auto& entt : entities)
		{
			Entity entity { entt, scene };
			const auto* hierarchy = scene->TryGetWith<HierarchyComponent>(entity);
			// Una entidad es raíz si NO tiene componente de jerarquía, 
			// o si lo tiene pero su Parent es nulo.
			bool isRoot = (hierarchy == nullptr) || (hierarchy->Parent == entt::null);
			if (isRoot)
			{
				OnHierarchyAdded(entity, L"");
			}
		}
		
		m_sceneDocument->GetScene()->OnComponentUpdated<NameComponent>().connect<&SceneHierarchyPanel::OnEntityNameUpdated>(this);
	}

	SceneHierarchyPanel::~SceneHierarchyPanel()
	{
		m_selectionService->SelectionChanged.disconnect(m_selectionChangedHandleId);
		m_sceneDocument->HierarchyChanged.disconnect(m_hierarchyChangedHandleId);
	}

	void SceneHierarchyPanel::OnHierarchyAdded(Entity entity, const std::wstring& parentKey)
	{
		// 1. Buenas prácticas: validar siempre los componentes que asumimos que existen
		std::wstring name = L"Unnamed Entity";
		if (entity.HasComponent<NameComponent>())
		{
			name = entity.GetComponent<NameComponent>().Name;
		}

		std::wostringstream builder;
		builder << parentKey << static_cast<uint32_t>(entity); // Casting explícito del entt::entity crudo
		auto key = builder.str();

		auto uuid = entity.GetUUID();
    
		// 2. Insertar en la UI
		auto node = m_treebox.Insert(key, name);
		node.SetUserData(uuid);

		m_entityToNodeMap[uuid] = node;

		// 3. Reconstruir los hijos solo si esta entidad tiene jerarquía
		if (entity.HasComponent<HierarchyComponent>())
		{
			auto& hierarchy = entity.GetComponent<HierarchyComponent>();
        
			// Iteramos directamente sobre la lista enlazada intrusiva
			entt::entity currentChild = hierarchy.FirstChild;
			while (currentChild != entt::null)
			{
				Entity childEntity { currentChild, m_sceneDocument->GetScene().get() };
          
				OnHierarchyAdded(childEntity, key + L"/");
				
				currentChild = childEntity.GetComponent<HierarchyComponent>().NextSibling;
			}
		}
	}

	void SceneHierarchyPanel::OnEntityNameUpdated(entt::registry& registry, entt::entity entityHandle)
	{
		auto& nameComp = registry.get<NameComponent>(entityHandle);
		auto& idComp = registry.get<IdComponent>(entityHandle);
		
		m_entityToNodeMap[idComp.Id].SetText(nameComp.Name);
	}
}
