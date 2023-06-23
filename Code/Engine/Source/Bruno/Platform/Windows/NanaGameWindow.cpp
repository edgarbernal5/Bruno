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

	Bruno::NanaGameWindow::~NanaGameWindow()
	{
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
			m_game->OnClientSizeChanged();
		});
	}

	nana::form& NanaGameWindow::GetForm()
	{
		return *m_form;
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
