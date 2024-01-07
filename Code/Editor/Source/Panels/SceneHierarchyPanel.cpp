#include "SceneHierarchyPanel.h"

#include "Bruno/Scene/Scene.h"
#include "Bruno/Scene/Entity.h"

namespace Bruno
{
	SceneHierarchyPanel::SceneHierarchyPanel(nana::window window, std::shared_ptr<Scene> scene) :
		nana::panel<true>(window),
		m_scene(scene)
	{
		this->caption("Hierarchy");

		m_treebox.create(*this);

		m_place.bind(*this);
		m_place.div("vert <tree margin=[4,4,4,4]>");

		m_place["tree"] << m_treebox;
		m_place.collocate();

		scene->SetHierarchyChangeCallback([&](Entity& entity, ActionMode actionMode) {
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
		if (hierarchy.Parent)
		{

		}
		builder << parentKey << (uint32_t)entity;

		auto key = builder.str();
		m_treebox.insert(key, name);
		for (UUID child : hierarchy.Children)
		{
			auto entity = m_scene->TryGetEntityWithUUID(child);
			OnHierarchyAdded(entity, key + "/");
		}
	}
}