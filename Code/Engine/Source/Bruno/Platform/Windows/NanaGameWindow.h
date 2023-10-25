#pragma once

#include "WindowsGameWindow.h"

#include <nana/gui.hpp>

#include <Bruno/Core/GameTimer.h>

namespace Bruno
{
	class Application;

	class NanaGameWindow : public GameWindow
	{
		BR_RTTI_DECLARATION(NanaGameWindow, GameWindow);

	public:
		NanaGameWindow(const GameWindowParameters& parameters, Application* game);
		~NanaGameWindow() = default;

		nana::form& GetForm();
		virtual WindowHandle GetHandle() override;
		virtual void Initialize() override;
		virtual int Run() override;

		virtual void Show() override;

	private:
		std::unique_ptr<nana::form> m_form;
		GameWindowParameters m_parameters;
		Application* m_application;
	};
}