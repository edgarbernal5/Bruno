#include "EditorGame.h"

#include "Bruno/Platform/Windows/NanaWindow.h"
#include "Panels/ScenePanel.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Bruno/Renderer/Model.h"
#include "Bruno/Scene/Scene.h"
#include "Content/EditorAssetManager.h"
#include "Bruno/Content/ContentTypeReaderManager.h"
#include <nana/gui/widgets/button.hpp>
#include <nana/debugger.hpp>
#include "Panels/ContentBrowserPanel.h"

namespace Bruno
{
#ifndef BR_SINGLE_THREAD_RENDERING
	void RenderTask(EditorGame& editor, std::atomic<bool>& exitRequested)
	{
		while (!exitRequested.load())
		{
			editor.OnTick();
			//std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
		BR_CORE_TRACE << "Exiting editor game loop..." << std::endl;
	}
#endif

	EditorGame::EditorGame(const ApplicationParameters& parameters, const std::wstring& projectPath) :
		Game(parameters),
		m_projectPath(projectPath)
	{
	}

	EditorGame::~EditorGame()
	{
#ifndef BR_SINGLE_THREAD_RENDERING
		m_exitRequested.store(true);
		m_workerThread.join();
#endif
	}

	void EditorGame::OpenDocument(const std::wstring& filename)
	{
		auto scene = std::make_shared<Scene>();

		auto scenePanel = m_place.add_float_pane<ScenePanel>("pane19", { 500,500 }, this, scene);

		auto model = m_assetManager->GetAsset<Model>(m_editorAssetManager->GetMetadata(filename).Handle);

		scene->InstantiateModel(model);
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
#ifndef BR_SINGLE_THREAD_RENDERING
			std::lock_guard lock{ panel->GetMutex() };
#endif
			panel->OnUpdate(timer);
			panel->OnDraw();
		}
	}

	void EditorGame::OnInitializeWindow(const WindowParameters& windowParameters)
	{
		m_window = std::make_unique<NanaWindow>(windowParameters, this);
		m_window->Initialize();
	}

	void EditorGame::OnRun()
	{
		Game::OnRun();

#ifndef BR_SINGLE_THREAD_RENDERING
		m_exitRequested.store(false);
		m_workerThread = std::thread(RenderTask, std::ref(*this), std::ref(m_exitRequested));
#endif
	}

	void EditorGame::AddScenePanel(ScenePanel* panel)
	{
		std::lock_guard lock{ m_scenePanelsMutex };
		
		panel->GetForm().events().enter_size_move([this](const nana::arg_size_move& args)
		{
			OnResizeMoveStarted();
		});
		panel->GetForm().events().exit_size_move([this](const nana::arg_size_move& args)
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
		{
			m_scenePanels.erase(it);
		}
	}

	void EditorGame::InitializeUI()
	{
		static int panelIdxx = 0;
		auto nanaWindow = m_window->As<NanaWindow>();
		
		nana::form& form = nanaWindow->GetForm();
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
			if (panelIdxx % 2 == 0) {
				auto panel = m_place.add_pane<nana::button>(panelIdxx == 0 ? "pane2" : (panelIdxx == 1 ? "pane3" : "pane4"), "pane1", panelIdxx % 2 == 0 ? nana::dock_position::down : nana::dock_position::right, std::string("A new pane is created."));
			}
			else {
				//auto panel = m_place.add_pane<ScenePanel>(panelIdxx == 0 ? "pane2" : (panelIdxx == 1 ? "pane3" : "pane4"), "pane1", panelIdxx % 2 == 0 ? nana::dock_position::down : nana::dock_position::right, this);

			}
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
			nana::debugger::enable_print_debug(!nana::debugger::is_enabled_print_debug());
		});

		auto scene = std::make_shared<Scene>();
		
		auto scenePanel = m_place.add_pane<ScenePanel>("pane1", "", nana::dock_position::right, this, scene);
		auto sceneHierarchyPanel = m_place.add_pane<SceneHierarchyPanel>("pane2", "pane1", nana::dock_position::right, scene, nullptr, nullptr);
		auto contentBrowser = m_place.add_pane<ContentBrowserPanel>("pane3", "pane1", nana::dock_position::down, m_applicationParameters.WorkingDirectory, this);

		auto model = m_assetManager->GetAsset<Model>(m_editorAssetManager->GetMetadata(L"Models\\Car\\Car.fbx").Handle);

		scene->InstantiateModel(model);

		m_place.collocate();

		form.events().key_release([this](const nana::arg_keyboard& args)
		{
			if (args.key == 'O')
			{
				if (panelIdxx % 2 == 0) {
					auto panel = m_place.add_pane<nana::button>(panelIdxx == 0 ? "pane2" : (panelIdxx == 1 ? "pane3" : (panelIdxx == 2 ? "pane4" : "pane5")), "pane1", panelIdxx % 2 == 0 ? nana::dock_position::down : nana::dock_position::right, std::string("A new pane is created."));
				}
				else {
					//auto panel = m_place.add_pane<ScenePanel>(panelIdxx == 0 ? "pane2" : (panelIdxx == 1 ? "pane3" : (panelIdxx == 2 ? "pane4" : "pane5")), "pane1", panelIdxx % 2 == 0 ? nana::dock_position::down : nana::dock_position::right, this);
				}
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

	void EditorGame::OnInitialize()
	{
		Game::OnInitialize();

		m_assetManager = std::make_shared<EditorAssetManager>(m_applicationParameters.WorkingDirectory);
		m_editorAssetManager = m_assetManager->As<EditorAssetManager>();
		InitializeUI();
	}
}
