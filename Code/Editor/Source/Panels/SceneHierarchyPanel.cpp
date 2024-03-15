#include "SceneHierarchyPanel.h"

#include <Bruno/Scene/Scene.h>
#include <Bruno/Scene/Entity.h>
#include "Scene/SceneDocument.h"
#include "Scene/SelectionService.h"

namespace Bruno
{
	SceneHierarchyPanel::SceneHierarchyPanel(nana::window window, std::shared_ptr<SceneDocument> sceneDocument) :
		nana::panel<true>(window),
		m_sceneDocument(sceneDocument)
	{
		this->caption("Hierarchy");

		m_selectionService = m_sceneDocument->GetSelectionService();

		m_treebox.create(*this);
		
		m_place.bind(*this);
		m_place.div("vert <tree>");

		m_place["tree"] << m_treebox;
		m_place.collocate();
		
		m_treebox.events().selected([&](const nana::arg_treebox& args)
		{
				BR_CORE_TRACE << "tree item selected: " << args.item.text() << ". " << args.operated << ". ignore events " << m_ignoreEvents << std::endl;

			if (m_ignoreEvents)
				return;

			std::vector<nana::treebox::item_proxy> node_selection;
			m_treebox.selected(node_selection);

			if (!args.operated)
			{
				m_selectionService->Deselect(args.item.value<UUID>());
				m_sceneDocument->UpdateSelection();
				return;
			}
			m_selectionService->Select(args.item.value<UUID>());
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

		m_treebox.enable_multiselection(true);
		m_treebox.use_select_contracted_parent_node(false);

		m_selectionChangedHandleId = m_selectionService->SelectionChanged.connect([&](const std::vector<UUID>& selection)
		{
			m_ignoreEvents = true;
			m_treebox.deselect_all();
			for (auto& uuid : selection)
			{
				m_entityToNodeMap[uuid].select(true);
			}
			m_ignoreEvents = false;
		});

		auto entities = sceneDocument->GetScene()->GetAllEntitiesWith<IdComponent, HierarchyComponent>();
		for (auto& ent : entities) {
			auto [idComponent, hierarchy] = entities.get<IdComponent, HierarchyComponent>(ent);
			if (!hierarchy.Parent)
			{
				OnHierarchyAdded(sceneDocument->GetScene()->GetEntityWithUUID(idComponent.Id));
			}
		}
	}

	SceneHierarchyPanel::~SceneHierarchyPanel()
	{
		m_selectionService->SelectionChanged.disconnect(m_selectionChangedHandleId);
		m_sceneDocument->HierarchyChanged.disconnect(m_hierarchyChangedHandleId);
	}

	void SceneHierarchyPanel::OnHierarchyAdded(Entity entity, const std::string& parentKey)
	{
		auto& hierarchy = entity.GetComponent<HierarchyComponent>();
		auto& name = entity.GetComponent<NameComponent>().Name;

		std::ostringstream builder;
		builder << parentKey << (uint32_t)entity;
		auto key = builder.str();

		auto node = m_treebox.insert(key, name);
		node.value(entity.GetUUID());

		m_entityToNodeMap[entity.GetUUID()] = node;

		for (UUID child : hierarchy.Children)
		{
			auto childEntity = m_sceneDocument->GetScene()->TryGetEntityWithUUID(child);
			if (childEntity)
			{
				OnHierarchyAdded(childEntity, key + "/");
			}
		}
	}
}