#include "brepch.h"
#include "EditorGame.h"

#include "Bruno/Platform/Windows/BertaWindow.h"
#include "Bruno/Renderer/Model.h"
#include "Bruno/Scene/Scene.h"
#include "Bruno/Content/ContentTypeReaderManager.h"
#include "Content/EditorAssetManager.h"

#include "Panels/Scene/SceneDocument.h"
#include "Panels/ScenePanel.h"
#include "Panels/SceneDocumentPanel.h"
#include "Panels/ContentBrowserPanel.h"

#include "AssetEditors/AssetEditorService.h"

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
		m_projectPath(projectPath),
		m_editorAssetManager(nullptr)
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

		//auto scenePanel = m_layout.add_float_pane<ScenePanel>("pane19", { 500,500 }, this, scene);

		auto model = m_assetManager->GetAsset<Model>(m_editorAssetManager->GetMetadata(filename).Handle);

		scene->InstantiateModel(model);
	}

	void EditorGame::OnGameLoop(const GameTimer& timer)
	{
#if 0
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
#endif
	}

	void EditorGame::OnInitializeWindow(const WindowParameters& windowParameters)
	{
		m_window = std::make_unique<BertaWindow>(windowParameters, this);
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
		
		panel->GetForm().GetEvents().EnterSizeMove.Connect([this](const Berta::ArgSizeMove& args)
			{
				OnResizeMoveStarted();
			});
		panel->GetForm().GetEvents().ExitSizeMove.Connect([this](const Berta::ArgSizeMove& args)
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
		auto nanaWindow = m_window->As<BertaWindow>();

		Berta::Form& form = nanaWindow->GetForm();
		m_layout.Create(form.Handle());
		m_menubar.Create(form.Handle());
		////////// VIEW
		m_layout.Parse("{VerticalLayout {menubar Height=25} {Dock dockRoot}}");
		m_layout.Attach("menubar", m_menubar);

		m_menubar.PushBack("&File");
		m_menubar.At(0).Append("Exit", [](Berta::MenuItem item)
			{
				Berta::GUI::Exit();
			});

		m_menubar.PushBack("&Edit");
		m_menubar.At(1).Append("Right panel", [this](Berta::MenuItem item)
			{
			});

		m_menubar.At(1).Append("Tab Panel", [this](Berta::MenuItem item)
			{
			});


		auto scene = std::make_shared<Scene>();
		auto sceneDocument = std::make_shared<SceneDocument>(scene, GetEditorAssetManager());
		auto model = m_assetManager->GetAsset<Model>(m_editorAssetManager->GetMetadata(L"Models\\Car\\Car.fbx").Handle);
		//auto model = m_assetManager->GetAsset<Model>(m_editorAssetManager->GetMetadata(L"Models\\sponza\\sponza.obj").Handle);
		sceneDocument->InstantiateModel(model);
		
		m_sceneDocumentPanel = std::make_unique<SceneDocumentPanel>(form, this, sceneDocument);

		m_contentBrowserPanel = std::make_unique<ContentBrowserPanel>(form, m_applicationParameters.WorkingDirectory,
			[](const std::wstring& filename)
			{
				//		//AssetEditor?
			});

		m_layout.AddPaneTab("scene-doc-pane", "tab-scene", std::move(m_sceneDocumentPanel), "", Berta::DockPosition::Tab);
		m_layout.AddPaneTab("content-browser-pane", "tab-content-browser", std::move(m_contentBrowserPanel), "scene-doc-pane", Berta::DockPosition::Right);

		m_layout.Apply();

		form.GetEvents().KeyReleased.Connect([this](const Berta::ArgKeyboard& args)
			{
			});

		form.GetEvents().EnterSizeMove.Connect([this](const Berta::ArgSizeMove& args)
			{
				//BR_CORE_TRACE << "enter_size_move / form." << std::endl;
			});

		form.GetEvents().ExitSizeMove.Connect([this](const Berta::ArgSizeMove& args)
			{
				//BR_CORE_TRACE << "exit_size_move / form." << std::endl;
			});

		form.GetEvents().Visibility.Connect([this](const Berta::ArgVisibility& args)
			{
				//BR_CORE_TRACE << "expose / form." << std::endl;
			});
	}

	void EditorGame::OnInitialize()
	{
		Game::OnInitialize();

		m_assetManager = std::make_shared<EditorAssetManager>(m_applicationParameters.WorkingDirectory);
		m_editorAssetManager = m_assetManager->As<EditorAssetManager>();
		InitializeUI();
	}
}
