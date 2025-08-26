#pragma once

#include <functional>
#include <string>
#include <Berta/Controls/Panel.h>
#include <Berta/GUI/Layout.h>
#include <Berta/Controls/TreeBox.h>
#include <Berta/Controls/ListBox.h>

#include <Berta/Controls/Menu.h>
#include "ContentBrowser/ContentBrowserItem.h"

namespace Bruno
{
	class EditorGame;

	class ContentBrowserPanel : public Berta::Panel
	{
	public:
		ContentBrowserPanel(Berta::Window* window, const std::wstring& workingDirectory, std::function<void(const std::wstring&)> selectItemCallback);

	private:
		void PopulateDirectory(Berta::TreeBoxItem node, const std::wstring& directory);
		void PopulateDirectoryInner(Berta::TreeBoxItem node, const std::wstring& directory);
		void PopulateFileDirectory(Berta::TreeBoxItem node);

		std::wstring m_workingDirectory;
		EditorGame* m_game;
		Berta::Layout m_layout;
		Berta::TreeBox m_treebox;
		Berta::ListBox m_listbox;

		Berta::Menu m_fileSelectionPopup;
		std::function<void(const std::wstring&)> m_selectItemCallback;
	};
}

