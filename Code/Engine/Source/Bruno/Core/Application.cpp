#include "brpch.h"
#include "Application.h"

namespace Bruno
{
	Application* Application::g_instance{ nullptr };

	Application::Application(const ApplicationParameters& parameters) :
		m_applicationParameters(parameters)
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
		windowParameters.Width = m_applicationParameters.WindowWidth;
		windowParameters.Height = m_applicationParameters.WindowHeight;
		windowParameters.Title = m_applicationParameters.Name;

		uint32_t dpi = GetDpiForSystem();
		float scaling_factor = static_cast<float>(dpi) / 96.0f;
		windowParameters.Width = static_cast<uint32_t>(scaling_factor * windowParameters.Width);
		windowParameters.Height = static_cast<uint32_t>(scaling_factor * windowParameters.Height);
		OnInitializeWindow(windowParameters);

		OnInitialize();
	}

	void Application::Run()
	{
		m_window->Show();

		OnRun();
		m_window->Run();
	}
}
