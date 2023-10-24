#pragma once

#include <Bruno.h>

#include <nana/gui/widgets/menubar.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <mutex>

#include "Pipeline/GameContentBuilder.h"

namespace Bruno
{
	class NanaGameWindow;

	class ContentPipelineGame : public Game
	{
	public:
		ContentPipelineGame(const ApplicationParameters& parameters);
		~ContentPipelineGame();
		
	protected:
		void InitializeUI();
		void OnInitialize(const GameWindowParameters& windowParameters) override;
		void OnGameLoop(const GameTimer& timer) override;

	private:
		nana::menubar		m_menubar;
		nana::place			m_place;

		GameContentBuilder	m_contentBuilder;
	};
}