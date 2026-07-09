#include "brepch.h"
#include "SceneHierarchyPanel.h"

#include <Bruno/Scene/Scene.h>
#include <Bruno/Scene/Entity.h>
#include "Scene/SceneDocument.h"
#include "Scene/SceneHierarchy.h"
#include "Scene/SelectionService.h"

namespace Bruno
{
	SceneHierarchyPanel::SceneHierarchyPanel(Berta::Window* window, std::shared_ptr<SceneDocument> sceneDocument) :
		Berta::Panel(window),
		m_sceneDocument(sceneDocument)
	{
		this->SetCaption("Hierarchy");

		m_selectionService = m_sceneDocument->GetSelectionService();
		m_sceneHierarchy = m_sceneDocument->GetSceneHierarchy();

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

			m_selectionService->DeselectAll();
			m_selectionService->Select(args.Items[0].GetUserData<UUID>());
			m_sceneDocument->UpdateSelection();
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
		//m_treebox.use_select_contracted_parent_node(false);

		m_selectionChangedHandleId = m_selectionService->SelectionChanged.connect([&](const std::vector<UUID>& selection)
		{
			m_ignoreEvents = true;
			m_treebox.DeselectAll();
			for (auto& uuid : selection)
			{
				m_entityToNodeMap[uuid].Select();
			}
			m_ignoreEvents = false;
		});

		auto entities = sceneDocument->GetScene()->GetAllEntitiesWith<IdComponent, HierarchyComponent>();
		for (auto& ent : entities)
		{
			auto [idComponent, hierarchy] = entities.get<IdComponent, HierarchyComponent>(ent);
			if (!hierarchy.Parent)
			{
				OnHierarchyAdded(sceneDocument->GetScene()->GetEntityWithUUID(idComponent.Id));
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
		auto& hierarchy = entity.GetComponent<HierarchyComponent>();
		auto& name = entity.GetComponent<NameComponent>().Name;

		std::wostringstream builder;
		builder << parentKey << static_cast<uint32_t>(entity);
		auto key = builder.str();

		auto uuid = entity.GetUUID();
		auto node = m_treebox.Insert(key, name);
		node.SetUserData(uuid);

		m_entityToNodeMap[uuid] = node;

		for (UUID child : hierarchy.Children)
		{
			auto childEntity = m_sceneDocument->GetScene()->TryGetEntityWithUUID(child);
			if (childEntity)
			{
				OnHierarchyAdded(childEntity, key + L"/");
			}
		}
	}

	void SceneHierarchyPanel::OnEntityNameUpdated(entt::registry& registry, entt::entity entityHandle)
	{
		Entity entity{ entityHandle, m_sceneDocument->GetScene().get() };
		
		auto& nameComp = registry.get<NameComponent>(entityHandle);
		auto& idComp = registry.get<IdComponent>(entityHandle);
		
		m_entityToNodeMap[idComp.Id].SetText(nameComp.Name);
	}
}
