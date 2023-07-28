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
			std::lock_guard lock{ panel->GetMutex() };
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

	//void EditorGame::InitializeUI()
	//{
	//	auto nanaGameWindow = reinterpret_cast<NanaGameWindow*>(m_gameWindow.get());

	//	nana::form& form = nanaGameWindow->GetForm();
	//	m_place.bind(form.handle());
	//	////////// VIEW
	//	m_place.div("dock<pane1>");

	//	m_place.dock<ScenePanel>("pane1", "f1", this);
	//	auto panel = reinterpret_cast<ScenePanel*>( m_place.dock_create("f1"));

	//	m_place.collocate();

	//	form.events().key_release([this](const nana::arg_keyboard& args) {
	//		if (args.key == 'O')
	//		{
	//			//auto panel = m_dockPlace.add_pane<nana::button>("pane2", "Scene right", "pane1", nana::dock_position::right, std::string("this"));
	//			auto panel = m_dockPlace.add_pane<ScenePanel>("pane2", "Scene right", "pane1", nana::dock_position::right, this);
	//			m_dockPlace.collocate();
	//			AddScenePanel(panel);
	//		}
	//		else if (args.key == 'P')
	//		{
	//			//auto panel = m_dockPlace.add_pane<nana::button>("pane1", "Scene tab", "", nana::dock_position::tab, std::string("this2"));
	//			auto panel = m_dockPlace.add_pane<ScenePanel>("pane1", "Scene tab", "", nana::dock_position::tab, this);
	//			m_dockPlace.collocate();
	//			AddScenePanel(panel);
	//		}
	//	});

	//	form.events().enter_size_move([this](const nana::arg_size_move& args)
	//		{
	//			//BR_CORE_TRACE << "enter_size_move / form." << std::endl;
	//		});

	//	form.events().exit_size_move([this](const nana::arg_size_move& args)
	//		{
	//			//BR_CORE_TRACE << "exit_size_move / form." << std::endl;
	//		});

	//	form.events().expose([this](const nana::arg_expose& args)
	//		{
	//			//BR_CORE_TRACE << "expose / form." << std::endl;
	//		});

	//	AddScenePanel(panel);
	//}

	void EditorGame::InitializeUI()
	{
		static int panelIdxx = 0;
		auto nanaGameWindow = m_gameWindow->As<NanaGameWindow>();
		
		nana::form& form = nanaGameWindow->GetForm();
		m_place.bind(form.handle());
		m_menubar.create(form.handle());
		////////// VIEW
		m_place.div("vert <menubar weight=25> <dock>");
		m_place["menubar"] << m_menubar;

		m_menubar.push_back("&File");
		m_menubar.at(0).append("Exit", [](nana::menu::item_proxy& ip)
		{
			nana::API::exit_all();
		});

		m_menubar.push_back("&Edit");
		m_menubar.at(1).append("Right panel", [this](nana::menu::item_proxy& ip)
		{
			auto panel = m_place.add_pane<nana::button>(panelIdxx == 0 ? "pane2" : (panelIdxx == 1 ? "pane3" : "pane4"), "pane1", panelIdxx %2 == 0 ? nana::dock_position::down : nana::dock_position::right, std::string("A new pane is created."));
			//auto panel = m_dockPlace.add_pane<ScenePanel>(panelIdxx == 0 ? "pane2" : (panelIdxx == 1 ? "pane3" : "pane4"), "pane1", panelIdxx % 2 == 0 ? nana::dock_position::down : nana::dock_position::right, this);
			m_place.collocate();
			//AddScenePanel(panel);
			panelIdxx++;
		});

		m_menubar.at(1).append("Tab Panel", [this](nana::menu::item_proxy& ip)
		{
			
		});

		m_menubar.push_back("Debug");
		m_menubar.at(2).append("Enable Printing", [this](nana::menu::item_proxy& ip)
		{
			m_place.enable_print_debug(!m_place.is_enabled_print_debug());
		});

		//auto panel = m_dockPlace.add_pane<nana::button>("pane1", "Scene main", "", nana::dock_position::right, std::string("This is the main scene\nEnjoy!"));
		auto panel = m_place.add_pane<ScenePanel>("pane1", "", nana::dock_position::right, this);

		m_place.collocate();

		form.events().key_release([this](const nana::arg_keyboard& args) {
			if (args.key == 'O')
			{
				auto panel = m_place.add_pane<nana::button>(panelIdxx == 0 ? "pane2" : (panelIdxx == 1 ? "pane3" : "pane4"), "pane1", panelIdxx %2 == 0 ? nana::dock_position::down : nana::dock_position::right, std::string("A new pane is created."));
				//auto panel = m_dockPlace.add_pane<ScenePanel>(panelIdxx == 0 ? "pane2" : (panelIdxx == 1 ? "pane3" : "pane4"), "pane1", panelIdxx % 2 == 0 ? nana::dock_position::down : nana::dock_position::right, this);
				m_place.collocate();
				//AddScenePanel(panel);
				panelIdxx++;
			}
			else if (args.key == 'P')
			{
				auto panel = m_place.add_pane<nana::button>("pane1", "", nana::dock_position::tab, std::string("This is a scene tab!!\nTow."));
				//auto panel = m_dockPlace.add_pane<ScenePanel>("pane1", "", nana::dock_position::tab, this);
				m_place.collocate();
				//AddScenePanel(panel);
			}
		});

		form.events().enter_size_move([this](const nana::arg_size_move& args)
		{
			//BR_CORE_TRACE << "enter_size_move / form." << std::endl;
		});

		form.events().exit_size_move([this](const nana::arg_size_move& args)
		{
			//BR_CORE_TRACE << "exit_size_move / form." << std::endl;
		});

		form.events().expose([this](const nana::arg_expose& args)
		{
			//BR_CORE_TRACE << "expose / form." << std::endl;
		});

		//AddScenePanel(panel);
	}
	//void EditorGame::InitializeUI()
	//{
	//	auto nanaGameWindow = reinterpret_cast<NanaGameWindow*>(m_gameWindow.get());

	//	nana::form& form = nanaGameWindow->GetForm();
	//	m_dockPlace.bind(form.handle());
	//	//m_panelMargin.create(form.handle());
	//	////////// VIEW
	//	//m_place.div("vert <menubar weight=25> vert <dock<pane1>>");
	//	//m_dockPlace.bind(m_panelMargin);
	//	m_dockPlace.div("dock");

	//	//auto panel = m_dockPlace.add_pane<nana::button>("pane1", "Scene main", "", nana::dock_position::right, std::string("this mainaniania\njsdksjd"));
	//	auto panel = m_dockPlace.add_pane<ScenePanel>("pane1", "Scene cap", "", nana::dock_position::right, this);

	//	m_dockPlace.collocate();

	//	form.events().key_release([this](const nana::arg_keyboard& args) {
	//		if (args.key == 'O')
	//		{
	//			//auto panel = m_dockPlace.add_pane<nana::button>("pane2", "Scene right", "pane1", nana::dock_position::right, std::string("this sdlajsdlkja lskjdla kjs1"));
	//			auto panel = m_dockPlace.add_pane<ScenePanel>("pane2", "Scene right", "pane1", nana::dock_position::right, this);
	//			m_dockPlace.collocate();
	//			AddScenePanel(panel);
	//		}
	//		else if (args.key == 'P')
	//		{
	//			auto panel = m_dockPlace.add_pane<nana::button>("pane1", "Scene tab", "", nana::dock_position::tab, std::string("this asf sfsf sdf sd2222"));
	//			//auto panel = m_dockPlace.add_pane<ScenePanel>("pane1", "Scene tab", "", nana::dock_position::tab, this);
	//			m_dockPlace.collocate();
	//			//AddScenePanel(panel);
	//		}
	//	});

	//	form.events().enter_size_move([this](const nana::arg_size_move& args)
	//		{
	//			//BR_CORE_TRACE << "enter_size_move / form." << std::endl;
	//		});

	//	form.events().exit_size_move([this](const nana::arg_size_move& args)
	//		{
	//			//BR_CORE_TRACE << "exit_size_move / form." << std::endl;
	//		});

	//	form.events().expose([this](const nana::arg_expose& args)
	//		{
	//			//BR_CORE_TRACE << "expose / form." << std::endl;
	//		});

	//	AddScenePanel(panel);
	//}

	//void EditorGame::InitializeUI()
	//{
	//	auto nanaGameWindow = reinterpret_cast<NanaGameWindow*>(m_gameWindow.get());

	//	nana::form& form = nanaGameWindow->GetForm();
	//	m_place.bind(form.handle());
	//	m_menubar.create(form.handle());

	//	m_menubar.push_back("&File");
	//	m_menubar.at(0).append("Exit", [](nana::menu::item_proxy& ip)
	//	{
	//		nana::API::exit_all();
	//	});

	//	m_menubar.push_back("&Edit");
	//	m_menubar.at(1).append("Right panel", [this](nana::menu::item_proxy& ip)
	//	{
	//		auto panel = m_dockPlace.add_pane<ScenePanel>("pane2", "Scene right", "pane1", nana::dock_position::right, this);
	//		
	//		//m_dockPlace.dock<ScenePanel>("pane1", "f1", this);
	//		//auto panel = reinterpret_cast<ScenePanel*>(m_dockPlace.add_pane("pane1", "f1", "Scene cap", "", nana::dock_position::right));
	//		m_dockPlace.collocate();
	//		AddScenePanel(panel);
	//	});

	//	m_menubar.at(1).append("Tab Panel", [this](nana::menu::item_proxy& ip)
	//	{
	//		auto panel = m_dockPlace.add_pane<ScenePanel>("pane1", "Scene tab", "", nana::dock_position::tab, this);
	//		
	//		//m_dockPlace.dock<ScenePanel>("pane1", "f1", this);
	//		//auto panel = reinterpret_cast<ScenePanel*>(m_dockPlace.add_pane("pane1", "f1", "Scene cap", "", nana::dock_position::right));
	//		m_dockPlace.collocate();
	//		AddScenePanel(panel);
	//	});
	//	////////// VIEW
	//	//m_place.div("vert <menubar weight=25> vert <dock<pane1>>");
	//	m_place.div("vert <menubar weight=25> _docking_");
	//	m_place["menubar"] << m_menubar;

	//	m_panelMargin.create(form.handle());

	//	m_dockPlace.bind(m_panelMargin);
	//	//m_dockPlace.div("margin=5 gap=2 dock");
	//	m_dockPlace.div("dock");

	//	m_place["_docking_"] << m_panelMargin;
	//	
	//	////////////////////////////
	//	/*
	//	* FUNCIONÓ v2
	//	* 
	//	m_place.div("vert <menubar weight=25> vert _docking_");
	//	m_place["menubar"] << m_menubar;

	//	m_panelMargin.create(form.handle());
	//	
	//	m_panelMarginPlace.bind(m_panelMargin);
	//	m_panelMarginPlace.div("margin=5 gap=2 _field_");

	//	m_place["_docking_"] << m_panelMargin;

	//	m_panelView.create(m_panelMargin);
	//	m_dockPlace.bind(m_panelView);

	//	m_panelMarginPlace["_field_"] << m_panelView;

	//	//m_dockPlace.div("<dock<pane1>>");
	//	m_dockPlace.div("dock");
	//	m_place.collocate();
	//	m_panelMarginPlace.collocate();
	//	*/

	//	/*
	//	FUNCIONÓ

	//	//m_place.div("vert <menubar weight=25> vert <dock<pane1>>");
	//	m_place.div("vert <menubar weight=25> <docking>");
	//	m_place["menubar"] << m_menubar;

	//	m_panelMargin.create(form.handle());

	//	m_dockPlace.bind(m_panelMargin);
	//	m_dockPlace.div("margin=5 gap=2 vert <dock<pane1>>");

	//	m_place["docking"] << m_panelMargin;

	//	*/
	//	/* Ejemplo para tener un pane con dos vistas diferentes con un tabbar para switchear entre ellas.
	//	m_place->div("vert <menubar weight=25>|<dock <pane1>>");
	//	m_place->dock<nana::button>("pane1", "f1", std::string("Button1"));
	//	m_place->dock<nana::button>("pane1", "f2", std::string("Button2"));
	//	m_place->dock_create("f1");
	//	m_place->dock_create("f2");
	//	*/

	//	//m_dockPlace.dock<ScenePanel>("pane1", "f1", this);
	//	//auto panel = reinterpret_cast<ScenePanel*>(m_dockPlace.dock_create("f1"));

	//	//auto panel = new ScenePanel(m_panelView, this);
	//	//m_dockPlace.add_pane("pane1", panel, "Scene cap", "", nana::dock_position::right);

	//	auto panel = m_dockPlace.add_pane<ScenePanel>("pane1", "Scene cap", "", nana::dock_position::right, this);

	//	//m_place.dock<ScenePanel>("pane2", "f3", this);
	//	//m_place.dock<nana::button>("pane1", "f2", std::string("Button2"));
	//	//auto aa=m_place.dock_create("f2");
	//	
	//	//m_place.dock<nana::button>("pane2", "f3", std::string("Button3"));
	//	//m_place.dock_create("f3");

	//	m_place.collocate();
	//	m_dockPlace.collocate();

	//	form.events().enter_size_move([this](const nana::arg_size_move& args)
	//	{
	//		//BR_CORE_TRACE << "enter_size_move / form." << std::endl;
	//	});

	//	form.events().exit_size_move([this](const nana::arg_size_move& args)
	//	{
	//		//BR_CORE_TRACE << "exit_size_move / form." << std::endl;
	//	});

	//	form.events().expose([this](const nana::arg_expose& args)
	//	{
	//		//BR_CORE_TRACE << "expose / form." << std::endl;
	//	});

	//	AddScenePanel(panel);
	//}
}
