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

	class ContentPipelineGame : public Game
	{
	public:
		ContentPipelineGame(const GameParameters& parameters);
		~ContentPipelineGame();
		
	protected:
		void InitializeUI();
		void OnInitialize(const GameWindowParameters& windowParameters) override;
		void OnGameLoop(const GameTimer& timer) override;
	};
}