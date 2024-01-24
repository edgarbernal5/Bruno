#include "ContentBrowserPanel.h"
#include <filesystem>

namespace Bruno
{
	ContentBrowserPanel::ContentBrowserPanel(nana::window window, const std::wstring& workingDirectory) : 
		nana::panel<true>(window),
		m_workingDirectory(workingDirectory)
	{
		this->caption("Content Browser");

		m_treebox.create(*this);
		m_listbox.create(*this);

		m_place.bind(*this);
		m_place.div("<tree> |70% <vert <list>");

		m_place["tree"] << m_treebox;
		m_place["list"] << m_listbox;
		m_place.collocate();

		PopulateDirectory(workingDirectory);
	}

	void ContentBrowserPanel::PopulateDirectory(const std::wstring& directoryPath)
	{
		m_treebox.auto_draw(false);
		PopulateDirectoryInner(directoryPath);
		m_treebox.auto_draw(true);
	}

	void ContentBrowserPanel::PopulateDirectoryInner(const std::wstring& directoryPath)
	{
		for (const auto& dir : std::filesystem::directory_iterator{ directoryPath })
		{
			if (!std::filesystem::is_directory(dir))
				continue;

			//auto Path = m_treebox.make_key_path(arg.item, "/") + "/";
			//m_treebox.insert(node, dir.path().filename().generic_string(), dir.path().filename().generic_string());

		}
	}
}