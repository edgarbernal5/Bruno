#include "ContentPipelineGame.h"

#include "Bruno/Platform/Windows/NanaGameWindow.h"

namespace Bruno
{
	ContentPipelineGame::ContentPipelineGame(const GameParameters& parameters) :
		Game(parameters)
	{
	}

	ContentPipelineGame::~ContentPipelineGame()
	{
	}

	void ContentPipelineGame::InitializeUI()
	{
	}

	void ContentPipelineGame::OnInitialize(const GameWindowParameters& windowParameters)
	{
		m_gameWindow = std::make_unique<NanaGameWindow>(windowParameters, this);
		m_gameWindow->Initialize();

		InitializeUI();

		m_gameWindow->Show();
	}

	void ContentPipelineGame::OnGameLoop(const GameTimer& timer)
	{
	}
}