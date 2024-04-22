#pragma once

#include <functional>
#include <string>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/treebox.hpp>
#include <nana/gui/widgets/listbox.hpp>

#include <nana/gui/widgets/menu.hpp>
#include "ContentBrowser/ContentBrowserItem.h"

namespace Bruno
{
	class EditorGame;

	class ContentBrowserPanel : public nana::panel<true>
	{
	public:
		ContentBrowserPanel(nana::window window, const std::wstring& workingDirectory, std::function<void(const std::wstring&)> selectItemCallback);

	private:
		void PopulateDirectory(nana::treebox::item_proxy& node, const std::wstring& directory);
		void PopulateDirectoryInner(nana::treebox::item_proxy& node, const std::wstring& directory);
		void PopulateFileDirectory(nana::treebox::item_proxy& node);

		std::wstring m_workingDirectory;
		EditorGame* m_game;
		nana::place m_place;
		nana::treebox m_treebox;
		nana::listbox m_listbox;

		nana::menu m_fileSelectionPopup;
		std::function<void(const std::wstring&)> m_selectItemCallback;
	};
}

