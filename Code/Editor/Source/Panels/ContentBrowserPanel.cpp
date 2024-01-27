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

		m_listbox.append_header("Name", 150);

		m_place.collocate();

		std::filesystem::directory_entry rootDirectoryEntry(m_workingDirectory);
		auto node = m_treebox.insert("Content", "Content");
		node.value(rootDirectoryEntry);
		PopulateDirectory(node, workingDirectory);

		m_treebox.events().selected([&](const nana::arg_treebox& arg)
		{
			m_listbox.auto_draw(false);
			PopulateFileDirectory(arg.item);
			m_listbox.auto_draw(true);
		});

		m_listbox.events().dbl_click([&](const nana::arg_mouse& args)
		{
			if (m_listbox.selected().size() == 0)
				return;
				
			for (auto& item : m_listbox.selected()) {

			}
		});

		m_fileSelectionPopup.append("Import asset", [](nana::menu::item_proxy& ip) {});
		m_fileSelectionPopup.append_splitter();
		m_fileSelectionPopup.append("Add to scene", [](nana::menu::item_proxy& ip) {});

		m_listbox.events().mouse_down([&](const nana::arg_mouse& args)
		{
			if (m_listbox.selected().size() == 0)
				return;

			menu_popuper(m_fileSelectionPopup)(args);
		});
	}

	void ContentBrowserPanel::PopulateDirectory(nana::treebox::item_proxy& node, const std::wstring& directoryPath)
	{
		m_treebox.auto_draw(false);
		PopulateDirectoryInner(node, directoryPath);
		m_treebox.auto_draw(true);
	}

	void ContentBrowserPanel::PopulateDirectoryInner(nana::treebox::item_proxy& node, const std::wstring& directoryPath)
	{
		for (const auto& directoryEntry : std::filesystem::directory_iterator{ directoryPath })
		{
			if (!std::filesystem::is_directory(directoryEntry))
				continue;

			auto child = m_treebox.insert(node, directoryEntry.path().filename().generic_string(), directoryEntry.path().filename().generic_string());
			child.value(directoryEntry);
			PopulateDirectoryInner(child, directoryEntry.path());
		}
	}

	void ContentBrowserPanel::PopulateFileDirectory(nana::treebox::item_proxy& node)
	{
		m_listbox.clear();
		auto& nodeDirectoryEntry = node.value<std::filesystem::directory_entry>();
		for (const auto& directoryEntry : std::filesystem::directory_iterator{ nodeDirectoryEntry.path()})
		{
			auto item = m_listbox.at(0).append(directoryEntry.path().filename(), true);
			item.value(directoryEntry);
		}
	}
}