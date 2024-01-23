#pragma once

#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/treebox.hpp>
#include <nana/gui/widgets/listbox.hpp>

namespace Bruno
{
	class ContentBrowserPanel : public nana::panel<true>
	{
	public:
		ContentBrowserPanel(nana::window window);

	private:

		nana::place m_place;
		nana::treebox m_treebox;
		nana::listbox m_listbox;
	};
}

