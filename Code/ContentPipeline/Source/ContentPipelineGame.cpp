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
		auto nanaGameWindow = m_gameWindow->As<NanaGameWindow>();
		nana::form& form = nanaGameWindow->GetForm();
		m_place.bind(form.handle());
		m_menubar.create(form.handle());
		////////// VIEW
		m_place.div("vert <menubar weight=45>");
		m_place["menubar"] << m_menubar;

		m_menubar.push_back("&File");
		m_menubar.at(0).append("Select folder", [](nana::menu::item_proxy& ip)
		{
			
		});
		m_menubar.at(0).append("Exit", [](nana::menu::item_proxy& ip)
		{
			nana::API::exit_all();
		});

		m_place.collocate();
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