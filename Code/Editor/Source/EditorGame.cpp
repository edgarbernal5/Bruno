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

		m_gameWindow = std::make_unique<NanaGameWindow>(windowParameters, this);
		m_gameWindow->Initialize();
		m_nanaGameWindow = reinterpret_cast<NanaGameWindow*>(m_gameWindow.get());

		InitUI();

		m_gameWindow->Show();
	}

	void EditorGame::OnUpdate(const GameTimer& timer)
	{
		std::vector<ScenePanel*> temp;
		{
			std::lock_guard lock{ m_scenePanelsMutex };
			temp.reserve(m_scenePanels.size());
			temp.assign(m_scenePanels.begin(), m_scenePanels.end());
		}
		for (auto panel : m_scenePanels)
		{
			panel->OnUpdate(timer);
		}
	}

	void EditorGame::OnDraw()
	{
		std::vector<ScenePanel*> temp;
		{
			std::lock_guard lock{ m_scenePanelsMutex };
			temp.reserve(m_scenePanels.size());
			temp.assign(m_scenePanels.begin(), m_scenePanels.end());
		}
		for (auto panel : temp)
		{
			panel->OnDraw();
		}
	}

	void EditorGame::AddScenePanel(ScenePanel* panel)
	{
		std::lock_guard lock{ m_scenePanelsMutex };
		m_scenePanels.push_back(panel);
	}

	void EditorGame::RemoveScenePanel(ScenePanel* panel)
	{
		std::lock_guard lock{ m_scenePanelsMutex };

		auto it = std::find(m_scenePanels.begin(), m_scenePanels.end(), panel);
		if (it != m_scenePanels.end())
			m_scenePanels.erase(it);
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

		m_place.dock<ScenePanel>("pane1", "f1", this);
		m_place.dock<ScenePanel>("pane1", "f2", this);
		//m_place.dock<nana::button>("pane1", "f2", std::string("Button2"));
		m_place.dock_create("f1");
		m_place.dock_create("f2");
		m_place.dock<nana::button>("pane2", "f3", std::string("Button3"));
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

		form.events().activate([this](const nana::arg_activate& args)
		{
			BR_CORE_TRACE << "activate / form. " << args.window_handle << std::endl;
		});
		form.events().expose([this](const nana::arg_expose& args)
		{
			BR_CORE_TRACE << "expose / form." << std::endl;
		});
	}
}
