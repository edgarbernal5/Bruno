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
}
