#include "ContentBrowserPanel.h"

namespace Bruno
{
	ContentBrowserPanel::ContentBrowserPanel(nana::window window) : 
		nana::panel<true>(window)
	{
		this->caption("Content Browser");

		m_treebox.create(*this);
		m_listbox.create(*this);

		m_place.bind(*this);
		m_place.div("<tree> |70% <vert <list>");

		m_place["tree"] << m_treebox;
		m_place["list"] << m_listbox;
		m_place.collocate();
	}
}