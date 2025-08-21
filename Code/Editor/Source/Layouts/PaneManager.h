#pragma once

#include <Berta/GUI/Layout.h>

namespace Bruno
{
	class PaneManager
	{
	public:
		PaneManager(Berta::Layout* place);

		//template<typename Panel, typename ...Args>
		//Panel* AddPane(const std::string& dock_id, const std::string& relative_pane_id, Berta::dock_position dock_position, Args&& ... args)
		//{
		//	return m_place->add_pane(dock_id, relative_pane_id, dock_position, args);
		//}

	private:
		Berta::Layout* m_place;
	};
}