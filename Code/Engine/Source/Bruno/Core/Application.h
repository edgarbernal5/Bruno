#pragma once

#include "Bruno/Core/Base.h"
#include "Bruno/Core/Window.h"

namespace Bruno
{
	struct ApplicationParameters
	{
		std::string Name = "Bruno";
		uint32_t WindowWidth = 800;
		uint32_t WindowHeight = 600;
		std::wstring WorkingDirectory;
	};

	class Application
	{
	public:
		virtual ~Application();

		void Initialize();
		void Run();

		static Application* GetInstance() { return g_instance; }

		friend class NanaWindow;
		friend class WindowsGameWindow;
	protected:
		Application(const ApplicationParameters& paramteres);

		virtual void OnInitialize() = 0;
		virtual void OnInitializeWindow(const WindowParameters& windowParameters) = 0;
		virtual void OnRun() = 0;

		virtual void OnResize() {}
		virtual void OnActivated() {}
		virtual void OnDeactivated() {}
		virtual void OnResizeMoveStarted() {}
		virtual void OnResizeMoveFinished() {}

		virtual void OnMouseDown(MouseButtonState btnState, int x, int y) {}
		virtual void OnMouseMove(MouseButtonState btnState, int x, int y) {}
		virtual void OnMouseUp(MouseButtonState btnState, int x, int y) {}
		virtual void OnMouseWheel(MouseButtonState btnState, int x, int y, int wheelDelta) {}
		virtual void OnKeyPressed(KeyCode key, KeyboardState state) {}
		virtual void OnKeyReleased(KeyCode key, KeyboardState state) {}

		std::unique_ptr<Window> m_window;
		ApplicationParameters m_applicationParameters;

		static Application* g_instance;
	private:
	};

	Application* CreateApplication(int argc, char** argv);
}

