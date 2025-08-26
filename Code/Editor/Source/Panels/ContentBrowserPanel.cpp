#include "brepch.h"
#include "ContentBrowserPanel.h"

#include "Bruno/Core/Base.h"
#include "EditorGame.h"

namespace Bruno
{
	ContentBrowserPanel::ContentBrowserPanel(Berta::Window* window, const std::wstring& workingDirectory, std::function<void(const std::wstring&)> selectItemCallback) :
		Berta::Panel(window),
		m_workingDirectory(workingDirectory),
		m_selectItemCallback(selectItemCallback)
	{
		this->SetCaption("Content Browser");

		m_treebox.Create(*this);
		m_listbox.Create(*this);

		m_layout.Create(*this);
		m_layout.Parse("{HorizontalLayout {tree} |70% {VerticalLayout {list}}");

		m_layout.Attach("tree", m_treebox);
		m_layout.Attach("list", m_listbox);

		m_listbox.AppendHeader("Name", 150);

		m_layout.Apply();

		std::filesystem::directory_entry rootDirectoryEntry(m_workingDirectory);
		auto rootNode = m_treebox.Insert("Content", "Content");
		rootNode.SetUserData(rootDirectoryEntry);
		PopulateDirectory(rootNode, workingDirectory);

		m_treebox.GetEvents().Selected.Connect([&](const Berta::ArgTreeBoxSelection& arg)
			{
				m_listbox.SetAutoDraw(false);
				PopulateFileDirectory(arg.Items[0]);
				m_listbox.SetAutoDraw(true);
			});

		m_listbox.GetEvents().Selected.Connect([&](const Berta::ArgListBox& args)
			{
				//BR_CORE_TRACE << "listbox item selected: " << args.item.value<ContentBrowserItem>().DirectoryEntry.path() << std::endl;
			});

		m_listbox.GetEvents().DblClick.Connect([&](const Berta::ArgMouse& args)
			{
				auto selectedItems = m_listbox.GetSelected();
				if (selectedItems.size() == 0)
					return;

				for (auto& item : selectedItems)
				{
					auto& contentItem = item.GetUserData<ContentBrowserItem>();

					if (contentItem.IsDirectory)
					{
						auto path = m_treebox.GetKeyPath(contentItem.TreeNode, '/') + ("/") + contentItem.DirectoryEntry.path().filename().generic_string();
						m_treebox.Find(path).Select();
					}
					else
					{
						m_selectItemCallback(contentItem.DirectoryEntry.path());
					}
				}
			});

		m_fileSelectionPopup.Append("Import new asset", [](Berta::MenuItem& ip) {});
		m_fileSelectionPopup.AppendSeparator();
		m_fileSelectionPopup.Append("Reimport asset", [](Berta::MenuItem& ip) {});

		m_listbox.GetEvents().MouseDown.Connect([&](const Berta::ArgMouse& args)
			{
				if (m_listbox.GetSelected().size() == 0)
					return;

				//menu_popuper(m_fileSelectionPopup)(args);
			});

		rootNode.Select();
	}

	void ContentBrowserPanel::PopulateDirectory(Berta::TreeBoxItem node, const std::wstring& directoryPath)
	{
		m_treebox.SetAutoDraw(false);
		PopulateDirectoryInner(node, directoryPath);
		m_treebox.SetAutoDraw(true);
	}

	void ContentBrowserPanel::PopulateDirectoryInner(Berta::TreeBoxItem node, const std::wstring& directoryPath)
	{
		for (const auto& directoryEntry : std::filesystem::directory_iterator{ directoryPath })
		{
			if (!std::filesystem::is_directory(directoryEntry))
				continue;

			auto child = m_treebox.Insert(node, directoryEntry.path().filename().generic_string(), directoryEntry.path().filename().generic_string());
			child.SetUserData(directoryEntry);
			PopulateDirectoryInner(child, directoryEntry.path());
		}
	}

	void ContentBrowserPanel::PopulateFileDirectory(Berta::TreeBoxItem node)
	{
		m_listbox.Clear();
		auto& nodeDirectoryEntry = node.GetUserData<std::filesystem::directory_entry>();
		for (const auto& directoryEntry : std::filesystem::directory_iterator{ nodeDirectoryEntry.path()})
		{
			auto item = m_listbox.Append(directoryEntry.path().filename().string());
			item.SetText(0, directoryEntry.path().filename().string());
			
			ContentBrowserItem contentItem;
			contentItem.DirectoryEntry = directoryEntry;
			contentItem.IsDirectory = std::filesystem::is_directory(directoryEntry);
			contentItem.TreeNode = node;

			item.SetUserData(contentItem);
		}
	}
}