#include "SceneHierarchyPanel.h"

#include "Bruno/Scene/Scene.h"
#include "Bruno/Scene/Entity.h"
#include "Panels/Scene/SelectionService.h"
#include "Gizmos/GizmoService.h"

namespace Bruno
{
	SceneHierarchyPanel::SceneHierarchyPanel(nana::window window, std::shared_ptr<Scene> scene, std::shared_ptr<SelectionService> selectionService, std::shared_ptr<GizmoService> gizmoService) :
		nana::panel<true>(window),
		m_scene(scene),
		m_selectionService(selectionService),
		m_gizmoService(gizmoService)
	{
		this->caption("Hierarchy");

		m_treebox.create(*this);

		m_place.bind(*this);
		m_place.div("vert <tree>");

		m_place["tree"] << m_treebox;
		m_place.collocate();

		m_treebox.events().selected([&](const nana::arg_treebox& args)
		{
			BR_CORE_TRACE << "tree item selected: " << args.item.text() << ". " << args.operated << std::endl;
			if (!args.operated)
				return;

			UUID entityUUID = args.item.value<UUID>();
			if (entityUUID)
			{
				/*m_selectionService->Select(entityUUID);
				auto worldMatrix = m_scene->GetWorldSpaceMatrix(m_scene->GetEntityWithUUID(entityUUID));
				m_gizmoService->SetGizmoPosition(worldMatrix.Translation());*/
			}
			else
			{
				//m_selectionService->DeselectAll();
			}
			//m_gizmoService->SetActive(entityUUID);
		});

		scene->SetHierarchyChangeCallback([&](Entity& entity, ActionMode actionMode)
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
	}

	void SceneHierarchyPanel::OnHierarchyAdded(Entity& entity, const std::string& parentKey)
	{
		auto& hierarchy = entity.GetComponent<HierarchyComponent>();
		auto& name = entity.GetComponent<NameComponent>().Name;

		std::ostringstream builder;
		builder << parentKey << (uint32_t)entity;
		auto key = builder.str();

		auto node = m_treebox.insert(key, name);
		node.value(entity.GetUUID());

		m_entityTreeboxMap[entity.GetUUID()] = node;

		for (UUID child : hierarchy.Children)
		{
			auto childEntity = m_scene->TryGetEntityWithUUID(child);
			if (childEntity)
			{
				OnHierarchyAdded(childEntity, key + "/");
			}
		}
	}
}