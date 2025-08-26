#include "brcppch.h"
#include "ContentPipelineApplication.h"

#include "Pipeline/ProcessorManager.h"
#include "Pipeline/Serialization/ContentTypeWriterManager.h"

#include <Bruno/Platform/Windows/BertaWindow.h>
#include <Bruno/Platform/DirectX/Shader.h>
#include "Bruno/Platform/DirectX/Texture.h"
#include "Bruno/Platform/DirectX/GraphicsDevice.h"
#include "Bruno/Renderer/Model.h"
#include "Bruno/Content/ContentManager.h"
#include "Bruno/Content/ContentTypeReaderManager.h"

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
		auto bertaWindow = m_window->As<BertaWindow>();
		Berta::Form& form = bertaWindow->GetForm();
		m_layout.Create(form.Handle());
		m_menubar.Create(form.Handle());
		////////// VIEW
		m_layout.Parse("{VerticalLayout {menubar Height=45}");
		m_layout.Attach("menubar", m_menubar);

		auto& menuFile = m_menubar.PushBack("&File");
		menuFile.Append("Select folder", [&form, this](Berta::MenuItem& ip)
		{
			/*Berta::filebox fileBox(form, true);
			fileBox.add_filter("Shader File", "*.hlsl;*.fx");
			fileBox.add_filter("Image File", "*.bmp;*.jpg;*.dds");
			fileBox.add_filter("All Files", "*.*");

			auto selectedFiles = fileBox();
			if (!selectedFiles.empty())
			{
				std::wstring rootDirectory = m_applicationParameters.WorkingDirectory;
				GameContentBuilder::Settings settings{ rootDirectory };
				settings.OutputDirectory = rootDirectory;

				m_contentBuilder.SetSettings(settings);

				for (auto& file : selectedFiles)
				{
					auto relativePath = std::filesystem::relative(file, rootDirectory);
					m_contentBuilder.RequestBuild(file.c_str(), relativePath);
				}
			}*/
		});

		menuFile.Append("Build", [this](Berta::MenuItem& ip)
		{
			m_contentBuilder.Run();
		});

		menuFile.Append("Read", [&form, this](Berta::MenuItem& ip)
		{
			/*Berta::filebox fileBox(form, true);
			fileBox.add_filter("Shader File", "*.hlsl;*.fx");
			fileBox.add_filter("Image File", "*.bmp;*.jpg;*.dds");
			fileBox.add_filter("All Files", "*.*");

			auto selectedFiles = fileBox();
			if (!selectedFiles.empty())
			{
				std::wstring rootDirectory = m_applicationParameters.WorkingDirectory;

				for (auto& file : selectedFiles)
				{
					auto relativePath = std::filesystem::relative(file, rootDirectory);

					ContentManager manager(rootDirectory);

					auto typelessRtti = manager.Load<RTTI>(relativePath);
				}
			}*/
		});
		menuFile.AppendSeparator();
		menuFile.Append("Exit", [](Berta::MenuItem& ip)
		{
			Berta::GUI::Exit();
		});

		m_layout.Apply();
	}

	void ContentPipelineApplication::OnInitialize()
	{
		ProcessorManager::Initialize();
		ContentTypeWriterManager::Initialize();
		ContentTypeReaderManager::Initialize();

		m_device = GraphicsDevice::Create();
		Bruno::Graphics::GetDevice() = m_device.get();

		InitializeUI();
	}

	void ContentPipelineApplication::OnInitializeWindow(const WindowParameters& windowParameters)
	{
		m_window = std::make_unique<BertaWindow>(windowParameters, this);
		m_window->Initialize();
	}

	void ContentPipelineApplication::OnRun()
	{
	}
}