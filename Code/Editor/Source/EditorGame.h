#pragma once

#include <Bruno.h>
#include <Bruno/Core/UIApplication.h>
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
	class NanaGameWindow;
	class GraphicsDevice;
	class ScenePanel;

	class EditorGame : public UIApplication
	{
	public:
		EditorGame(const ApplicationParameters& parameters);
		~EditorGame();

		virtual void OnTick();

		friend class ScenePanel;
	protected:
		void InitializeUI();
		void OnInitialize() override;
		void OnInitializeWindow(const GameWindowParameters& windowParameters) override;
		void OnRun() override;
		void OnPostRun() override;
		void OnGameLoop(const GameTimer& timer);

	private:
		void AddScenePanel(ScenePanel* panel);
		void RemoveScenePanel(ScenePanel* panel);

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
	};
}