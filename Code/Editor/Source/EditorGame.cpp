#include "EditorGame.h"

#include "Bruno/Platform/Windows/NanaGameWindow.h"
#include "Panels/ScenePanel.h"
#include <nana/gui/widgets/button.hpp>

namespace Bruno
{
	EditorGame::EditorGame(const GameParameters& parameters) :
		Game(parameters)
	{
	}

	EditorGame::~EditorGame()
	{
	}

	void EditorGame::OnGameLoop(const GameTimer& timer)
	{
		std::vector<ScenePanel*> temp;
		{
			std::lock_guard lock{ m_scenePanelsMutex };
			temp.reserve(m_scenePanels.size());
			temp.assign(m_scenePanels.begin(), m_scenePanels.end());
		}
		for (auto panel : temp)
		{
			panel->OnUpdate(timer);
			panel->OnDraw();
		}
	}

	void EditorGame::OnInitialize(const GameWindowParameters& windowParameters)
	{
		m_gameWindow = std::make_unique<NanaGameWindow>(windowParameters, this);
		m_gameWindow->Initialize();

		InitializeUI();

		m_gameWindow->Show();
	}

	void EditorGame::AddScenePanel(ScenePanel* panel)
	{
		std::lock_guard lock{ m_scenePanelsMutex };
		
		panel->events().enter_size_move([this](const nana::arg_size_move& args)
		{
			OnResizeMoveStarted();
		});
		panel->events().exit_size_move([this](const nana::arg_size_move& args)
		{
			OnResizeMoveFinished();
		});

		m_scenePanels.push_back(panel);
	}

	void EditorGame::RemoveScenePanel(ScenePanel* panel)
	{
		std::lock_guard lock{ m_scenePanelsMutex };

		auto it = std::find(m_scenePanels.begin(), m_scenePanels.end(), panel);
		if (it != m_scenePanels.end())
			m_scenePanels.erase(it);
	}

	void EditorGame::InitializeUI()
	{
		auto nanaGameWindow = reinterpret_cast<NanaGameWindow*>(m_gameWindow.get());

		nana::form& form = nanaGameWindow->GetForm();
		m_place.bind(form.handle());
		m_menubar.create(form.handle());

		m_menubar.push_back("&File");
		m_menubar.at(0).append("Exit", [](nana::menu::item_proxy& ip)
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

		m_place.dock<ScenePanel>("pane1", "f1", this);
		m_place.dock<ScenePanel>("pane2", "f3", this);
		m_place.dock<nana::button>("pane1", "f2", std::string("Button2"));
		m_place.dock_create("f1");
		m_place.dock_create("f2");
		//m_place.dock<nana::button>("pane2", "f3", std::string("Button3"));
		m_place.dock_create("f3");

		m_place.collocate();

		form.events().enter_size_move([this](const nana::arg_size_move& args)
		{
			BR_CORE_TRACE << "enter_size_move / form." << std::endl;
		});

		form.events().exit_size_move([this](const nana::arg_size_move& args)
		{
			BR_CORE_TRACE << "exit_size_move / form." << std::endl;
		});

		form.events().expose([this](const nana::arg_expose& args)
		{
			BR_CORE_TRACE << "expose / form." << std::endl;
		});
	}
}
