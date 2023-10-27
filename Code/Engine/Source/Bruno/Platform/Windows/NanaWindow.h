#pragma once

#include "WindowsGameWindow.h"

#include <nana/gui.hpp>

#include <Bruno/Core/GameTimer.h>

namespace Bruno
{
	class Application;

	class NanaWindow : public Window
	{
		BR_RTTI_DECLARATION(NanaWindow, Window);

	public:
		NanaWindow(const WindowParameters& parameters, Application* application);
		~NanaWindow() = default;

		nana::form& GetForm();
		virtual WindowHandle GetHandle() override;
		virtual void Initialize() override;
		virtual int Run() override;

		virtual void Show() override;

	private:
		std::unique_ptr<nana::form> m_form;
		WindowParameters m_parameters;
		Application* m_application;
	};
}