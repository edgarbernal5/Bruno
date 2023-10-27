#include "brpch.h"
#include "Application.h"

namespace Bruno
{
	Application* Application::g_instance{ nullptr };

	Application::Application(const ApplicationParameters& parameters) :
		m_parameters(parameters)
	{
		g_instance = this;

		if (!parameters.WorkingDirectory.empty())
		{
			std::filesystem::current_path(parameters.WorkingDirectory);
		}
	}

	Application::~Application()
	{
	}

	void Application::Initialize()
	{
		WindowParameters windowParameters;
		windowParameters.Width = m_parameters.WindowWidth;
		windowParameters.Height = m_parameters.WindowHeight;
		windowParameters.Title = m_parameters.Name;
		OnInitializeWindow(windowParameters);

		OnInitialize();
	}

	void Application::Run()
	{
		m_gameWindow->Show();

		OnRun();
		m_gameWindow->Run();
	}
}
