#pragma once

#include "Bruno.h"
#include "Bruno/Core/GameTimer.h"

#include <nana/gui/widgets/menubar.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <mutex>

#include <thread>
#include <atomic>

namespace Bruno
{
	class NanaWindow;
	class GraphicsDevice;
	class ScenePanel;
	class EditorAssetManager;

	class EditorGame : public Game
	{
	public:
		EditorGame(const ApplicationParameters& parameters, const std::wstring& projectPath);
		~EditorGame();

		void AddScenePanel(ScenePanel* panel);
		void RemoveScenePanel(ScenePanel* panel);

		void OpenDocument(const std::wstring& filename);

		EditorAssetManager* GetEditorAssetManager() const {
			return m_editorAssetManager;
		}
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

		nana::menubar		m_menubar;
		nana::place			m_place;
		nana::panel<false>	m_panelMargin;
		//nana::br_place		m_panelMarginPlace;
		nana::panel<true>	m_panelView;

		std::vector<ScenePanel*> m_scenePanels;
		std::mutex m_scenePanelsMutex{};

		std::wstring m_projectPath;
		EditorAssetManager* m_editorAssetManager;
	};
}