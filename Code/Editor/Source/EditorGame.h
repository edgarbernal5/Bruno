#pragma once

#include <Bruno.h>

#include <nana/gui/widgets/menubar.hpp>
#include <nana/gui/place.hpp>
#include <mutex>

namespace Bruno
{
	class NanaGameWindow;
	class GraphicsDevice;
	class ScenePanel;

	class EditorGame : public Game
	{
	public:
		EditorGame(const GameParameters& parameters);
		~EditorGame();

		friend class ScenePanel;
	protected:
		void InitUI();
		virtual void DoOnInitialize() override;
		void OnUpdate(const GameTimer& timer) override;
		void OnDraw() override;

	private:
		NanaGameWindow* m_nanaGameWindow;
		nana::menubar m_menubar;
		nana::place m_place;
		std::vector<ScenePanel*> m_scenePanels;

		std::mutex m_scenePanelsMutex{};

		void AddScenePanel(ScenePanel* panel);
		void RemoveScenePanel(ScenePanel* panel);
	};
}