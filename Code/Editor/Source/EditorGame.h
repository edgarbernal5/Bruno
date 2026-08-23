#pragma once

#include "Bruno.h"
#include "Bruno/Core/GameTimer.h"

#include <Berta/Controls/MenuBar.h>
#include <Berta/GUI/Layout.h>
#include <mutex>

#include <thread>
#include <atomic>

namespace Bruno
{
	class ProfilerPanel;
	class BertaWindow;
	class GraphicsDevice;
	class ScenePanel;
	class EditorAssetManager;
	class AssetEditorService;
	class SceneDocumentPanel;
	class ContentBrowserPanel;

	class EditorGame : public Game
	{
	public:
		EditorGame(const ApplicationParameters& parameters, const std::wstring& projectPath);
		~EditorGame() override;

		void AddScenePanel(ScenePanel* panel);
		void RemoveScenePanel(ScenePanel* panel);

		void OpenDocument(const std::wstring& filename);

		EditorAssetManager* GetEditorAssetManager() const { return m_editorAssetManager; }
	protected:
		void InitializeUI();
		void OnInitialize() override;
		void OnInitializeWindow(const WindowParameters& windowParameters) override;
		void OnRun() override;
		void OnGameLoop(const GameTimer& timer) override;

	private:
		std::shared_ptr<GraphicsDevice> m_device;
		GameTimer m_timer;

		std::thread			m_workerThread;
		std::atomic<bool>	m_exitRequested;

		Berta::MenuBar		m_menubar;
		Berta::Layout			m_layout;

		std::unique_ptr<SceneDocumentPanel> m_sceneDocumentPanel;
		std::unique_ptr<ContentBrowserPanel> m_contentBrowserPanel;
		std::unique_ptr<ProfilerPanel> m_profilerPanel;
		std::vector<ScenePanel*> m_scenePanels;
		std::mutex m_scenePanelsMutex{};

		std::wstring m_projectPath;
		EditorAssetManager* m_editorAssetManager;
		std::shared_ptr<AssetEditorService> m_documentService;
	};
}