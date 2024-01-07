#pragma once

#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/treebox.hpp>

namespace Bruno
{
	class Scene;
	class Entity;
	enum class ActionMode;

	class SceneHierarchyPanel : public nana::panel<true>
	{
	public:
		SceneHierarchyPanel(nana::window window, std::shared_ptr<Scene> scene);

	private:
		void OnHierarchyAdded(Entity& entity, const std::string& parentKey="");

		nana::place m_place;
		nana::treebox m_treebox;
		std::shared_ptr<Scene> m_scene;
	};
}
