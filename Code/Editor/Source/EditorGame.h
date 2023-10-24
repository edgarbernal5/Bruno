#pragma once

#include <Bruno.h>

#include <nana/gui/widgets/menubar.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <mutex>

namespace Bruno
{
	class NanaGameWindow;
	class GraphicsDevice;
	class ScenePanel;

	class EditorGame : public Game
	{
	public:
		EditorGame(const ApplicationParameters& parameters);
		~EditorGame();

		friend class ScenePanel;
	protected:
		void InitializeUI();
		void OnInitialize(const GameWindowParameters& windowParameters) override;
		void OnGameLoop(const GameTimer& timer) override;

	private:
		void AddScenePanel(ScenePanel* panel);
		void RemoveScenePanel(ScenePanel* panel);

		nana::menubar		m_menubar;
		nana::place			m_place;
		nana::panel<false>	m_panelMargin;
		//nana::br_place		m_panelMarginPlace;
		nana::panel<true>	m_panelView;

		std::vector<ScenePanel*> m_scenePanels;
		std::mutex m_scenePanelsMutex{};
	};
}