#include "EditorGame.h"

#include "Bruno/Platform/Windows/NanaGameWindow.h"
#include "Panels/ScenePanel.h"
#include <nana/gui/widgets/button.hpp>

namespace Bruno
{
	EditorGame::EditorGame(const GameParameters& parameters) :
		Game(parameters),
		m_nanaGameWindow(nullptr)
	{
	}

	EditorGame::~EditorGame()
	{
	}

	void EditorGame::DoOnInitialize()
	{
		GameWindowParameters windowParameters;
		windowParameters.Width = m_parameters.WindowWidth;
		windowParameters.Height = m_parameters.WindowHeight;
		windowParameters.Title = m_parameters.Name;

		m_nanaGameWindow = new NanaGameWindow(windowParameters, this);
		m_gameWindow = m_nanaGameWindow;
		m_gameWindow->Initialize();

		InitUI();

		m_gameWindow->Show();
	}

	void EditorGame::InitUI()
	{
		nana::form& form = m_nanaGameWindow->GetForm();
		m_place.bind(form.handle());
		m_menubar.create(form.handle());

		m_menubar.push_back("&File");
		m_menubar.at(0).append("Exit", [this](nana::menu::item_proxy& ip)
			{
				nana::API::exit_all();
			});

		m_menubar.push_back("&Edit");

		m_place.div("vert <menubar weight=25>|vert <dock <bottom pane1><pane2>>");
		m_place["menubar"] << m_menubar;
		
		/* Ejemplo para tener un pane con dos vistas diferentes con un tabbar para switchear entre ellas.
		m_place->div("vert <menubar weight=25>|<dock <pane1>>");
		m_place->dock<nana::button>("pane1", "f1", std::string("Button1"));
		m_place->dock<nana::button>("pane1", "f2", std::string("Button2"));
		m_place->dock_create("f1");
		m_place->dock_create("f2");
		*/

		m_place.dock<ScenePanel>("pane1", "f1");
		m_place.dock<ScenePanel>("pane1", "f2");
		//m_place.dock<nana::button>("pane1", "f2", std::string("Button2"));
		m_place.dock_create("f1");
		m_place.dock_create("f2");
		m_place.dock<nana::button>("pane2", "f3", std::string("Button3"));
		m_place.dock_create("f3");

		m_place.collocate();
	}
}
