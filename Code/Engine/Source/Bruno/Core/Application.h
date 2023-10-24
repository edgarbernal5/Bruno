#pragma once

#include "Bruno/Core/GameWindow.h"

namespace Bruno
{
	struct ApplicationParameters
	{
		std::string Name = "Bruno";
		uint32_t WindowWidth = 800;
		uint32_t WindowHeight = 600;
		std::string WorkingDirectory;
	};

	class Application
	{
	public:
		Application(const ApplicationParameters& appParamteres);
		virtual void Initialize() = 0;
		virtual void Run() = 0;

		static Application* GetInstance() { return g_instance; }
	protected:
		std::unique_ptr<GameWindow> m_gameWindow;
		ApplicationParameters m_parameters;

	private:
		static Application* g_instance;
	};

	Application* CreateApplication(int argc, char** argv);
}

