#include "ContentPipelineGame.h"

#include "Pipeline/ProcessorManager.h"

#include <Bruno/Platform/Windows/NanaGameWindow.h>

#include <nana/gui/filebox.hpp>

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

		auto& menuFile = m_menubar.push_back("&File");
		menuFile.append("Select folder", [&form, this](nana::menu::item_proxy& ip)
		{
			nana::filebox fileBox(form, true);
			fileBox.add_filter("Shader File", "*.hlsl;*.fx");
			fileBox.add_filter("Image File", "*.bmp;*.jpg;*.dds");
			fileBox.add_filter("All Files", "*.*");

			auto selectedFiles = fileBox();
			if (!selectedFiles.empty())
			{
				std::wstring rootDirectory = selectedFiles[0].parent_path();
				GameContentBuilder::Settings settings{ rootDirectory };
				m_contentBuilder.SetSettings(settings);

				for (auto& file : selectedFiles)
				{
					auto relativePath = std::filesystem::relative(file, rootDirectory);
					m_contentBuilder.RequestBuild(file.c_str(), relativePath);
				}
			}
		});

		menuFile.append("Build", [this](nana::menu::item_proxy& ip)
		{
			m_contentBuilder.Run();
		});
		menuFile.append_splitter();
		menuFile.append("Exit", [](nana::menu::item_proxy& ip)
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

		ProcessorManager::Initialize();

		m_gameWindow->Show();
	}

	void ContentPipelineGame::OnGameLoop(const GameTimer& timer)
	{
	}
}