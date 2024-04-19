#pragma once

#include <nana/gui/place.hpp>

namespace Bruno
{
	class PaneManager
	{
	public:
		PaneManager(nana::place* place);

		template<typename Panel, typename ...Args>
		Panel* AddPane(const std::string& dock_id, const std::string& relative_pane_id, nana::dock_position dock_position, Args&& ... args)
		{
			return m_place->add_pane(dock_id, relative_pane_id, dock_position, args);
		}

	private:
		nana::place* m_place;
	};
}