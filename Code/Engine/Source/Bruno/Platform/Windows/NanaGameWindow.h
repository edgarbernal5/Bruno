#pragma once

#include "WindowsGameWindow.h"

#include <nana/gui.hpp>

namespace Bruno
{
	class Game;

	class NanaGameWindow : public GameWindow
	{
	public:
		NanaGameWindow(const GameWindowParameters& parameters, Game* game);
		~NanaGameWindow();

		virtual WindowHandle GetHandle() override;
		virtual void Initialize() override;
		virtual void Run() override;

		nana::form& GetForm();
	private:
		std::unique_ptr<nana::form> m_form;
		GameWindowParameters m_parameters;
		Game* m_game;
	};
}