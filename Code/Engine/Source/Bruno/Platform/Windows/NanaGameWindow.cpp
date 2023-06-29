#include "brpch.h"
#include "NanaGameWindow.h"

#include "Bruno/Core/Game.h"

#include <thread>
#include <atomic>

namespace Bruno
{
	void RenderTask(Game& game, std::atomic<bool>& exitRequested)
	{
		while (!exitRequested.load())
		{
			game.OnTick();
			//std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
	}

	Bruno::NanaGameWindow::NanaGameWindow(const GameWindowParameters& parameters, Game* game) :
		m_parameters(parameters),
		m_game(game),
		m_form(nullptr)
	{
		m_data.Width = parameters.Width;
		m_data.Height = parameters.Height;
	}

	nana::form& NanaGameWindow::GetForm()
	{
		return *m_form;
	}

	WindowHandle NanaGameWindow::GetHandle()
	{
		return reinterpret_cast<WindowHandle>(m_form->native_handle());
	}

	void NanaGameWindow::Initialize()
	{
		m_form = std::make_unique<nana::form>(nana::API::make_center(m_parameters.Width, m_parameters.Height));
		m_form->caption(m_parameters.Title);

		m_form->events().resized([this](const nana::arg_resized& args)
		{
			m_data.Height = args.height;
			m_data.Width = args.width;
			m_game->OnResize();
		});
		
		m_form->events().activate([this](const nana::arg_activate& args)
		{	
			if (args.activated)
			{
				m_game->OnActivated();
				return;
			}
			m_game->OnDeactivated();
		});

		m_form->events().enter_size_move([this](const nana::arg_size_move& args)
		{
			m_game->OnResizeMoveStarted();
		});

		m_form->events().exit_size_move([this](const nana::arg_size_move& args)
		{
			m_game->OnResizeMoveFinished();
		});

		m_form->events().mouse_down([this](const nana::arg_mouse& args)
		{
			MouseButtonState btnState{};
			btnState.LeftButton = args.left_button;
			btnState.RightButton = args.right_button;
			btnState.MiddleButton = args.mid_button;

			m_game->OnMouseDown(btnState, args.pos.x, args.pos.y);
		});

		m_form->events().mouse_move([this](const nana::arg_mouse& args)
		{
			MouseButtonState btnState{};
			btnState.LeftButton = args.left_button;
			btnState.RightButton = args.right_button;
			btnState.MiddleButton = args.mid_button;

			m_game->OnMouseMove(btnState, args.pos.x, args.pos.y);
		});

		m_form->events().mouse_up([this](const nana::arg_mouse& args)
		{
			MouseButtonState btnState{};
			btnState.LeftButton = args.left_button;
			btnState.RightButton = args.right_button;
			btnState.MiddleButton = args.mid_button;

			m_game->OnMouseUp(btnState, args.pos.x, args.pos.y);
		});
	}

	int NanaGameWindow::Run()
	{
		std::atomic<bool> exitRequested;
		exitRequested.store(false);
		std::thread workerThread(RenderTask, std::ref(*m_game), std::ref(exitRequested));

		nana::exec();

		exitRequested.store(true);
		workerThread.join();

		return 0;
	}

	void NanaGameWindow::Show()
	{
		m_form->show();
	}
}
