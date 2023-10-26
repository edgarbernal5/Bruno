#include "ContentPipelineApplication.h"

#include "Pipeline/ProcessorManager.h"
#include "Pipeline/Serialization/ContentTypeWriterManager.h"

#include <Bruno/Platform/Windows/NanaWindow.h>
#include <Bruno/Platform/DirectX/Shader.h>
#include <Bruno/Platform/DirectX/Texture.h>
#include <Bruno/Renderer/Model.h>
#include <Bruno/Content/ContentManager.h>

#include <nana/gui/filebox.hpp>

namespace Bruno
{
	ContentPipelineApplication::ContentPipelineApplication(const ApplicationParameters& parameters) :
		Application(parameters)
	{
	}

	ContentPipelineApplication::~ContentPipelineApplication()
	{
	}

	void ContentPipelineApplication::InitializeUI()
	{
		auto nanaGameWindow = m_gameWindow->As<NanaWindow>();
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
				settings.OutputDirectory = rootDirectory;

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

		menuFile.append("Read", [&form, this](nana::menu::item_proxy& ip)
		{
			nana::filebox fileBox(form, true);
			fileBox.add_filter("Shader File", "*.hlsl;*.fx");
			fileBox.add_filter("Image File", "*.bmp;*.jpg;*.dds");
			fileBox.add_filter("All Files", "*.*");

			auto selectedFiles = fileBox();
			if (!selectedFiles.empty())
			{
				std::wstring rootDirectory = selectedFiles[0].parent_path();

				for (auto& file : selectedFiles)
				{
					auto relativePath = std::filesystem::relative(file, rootDirectory);

					ContentManager manager(rootDirectory);
					//auto shader = manager.Load<Shader>(relativePath);
					//auto texture = manager.Load<Texture>(relativePath);
					//auto model= manager.Load<Model>(relativePath);
					auto typelessRtti = manager.Load<RTTI>(relativePath);
				}
			}
		});
		menuFile.append_splitter();
		menuFile.append("Exit", [](nana::menu::item_proxy& ip)
		{
			nana::API::exit_all();
		});

		m_place.collocate();
	}

	void ContentPipelineApplication::OnInitialize()
	{
		ProcessorManager::Initialize();
		ContentTypeWriterManager::Initialize();

		InitializeUI();
	}

	void ContentPipelineApplication::OnInitializeWindow(const GameWindowParameters& windowParameters)
	{
		m_gameWindow = std::make_unique<NanaWindow>(windowParameters, this);
		m_gameWindow->Initialize();
	}

	void ContentPipelineApplication::OnRun()
	{
	}
}