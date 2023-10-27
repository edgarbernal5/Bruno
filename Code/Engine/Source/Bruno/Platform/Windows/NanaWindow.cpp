#include "brpch.h"
#include "NanaWindow.h"

#include "Bruno/Core/Application.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(NanaWindow);

	Bruno::NanaWindow::NanaWindow(const WindowParameters& parameters, Application* application) :
		m_parameters(parameters),
		m_application(application),
		m_form(nullptr)
	{
		m_data.Width = parameters.Width;
		m_data.Height = parameters.Height;
	}

	nana::form& NanaWindow::GetForm()
	{
		return *m_form;
	}

	WindowHandle NanaWindow::GetHandle()
	{
		return reinterpret_cast<WindowHandle>(m_form->native_handle());
	}

	void NanaWindow::Initialize()
	{
		m_form = std::make_unique<nana::form>(nana::API::make_center(m_parameters.Width, m_parameters.Height));
		m_form->caption(m_parameters.Title);

		m_form->events().resized([this](const nana::arg_resized& args)
		{
			m_data.Height = args.height;
			m_data.Width = args.width;
			m_application->OnResize();
		});
		
		m_form->events().activate([this](const nana::arg_activate& args)
		{	
			if (args.activated)
			{
				m_application->OnActivated();
				return;
			}
			m_application->OnDeactivated();
		});

		m_form->events().enter_size_move([this](const nana::arg_size_move& args)
		{
			m_application->OnResizeMoveStarted();
		});

		m_form->events().exit_size_move([this](const nana::arg_size_move& args)
		{
			m_application->OnResizeMoveFinished();
		});

		m_form->events().mouse_down([this](const nana::arg_mouse& args)
		{
			MouseButtonState btnState{};
			btnState.LeftButton = args.left_button;
			btnState.RightButton = args.right_button;
			btnState.MiddleButton = args.mid_button;

			m_application->OnMouseDown(btnState, args.pos.x, args.pos.y);
		});

		m_form->events().mouse_move([this](const nana::arg_mouse& args)
		{
			MouseButtonState btnState{};
			btnState.LeftButton = args.left_button;
			btnState.RightButton = args.right_button;
			btnState.MiddleButton = args.mid_button;

			m_application->OnMouseMove(btnState, args.pos.x, args.pos.y);
		});

		m_form->events().mouse_up([this](const nana::arg_mouse& args)
		{
			MouseButtonState btnState{};
			btnState.LeftButton = args.left_button;
			btnState.RightButton = args.right_button;
			btnState.MiddleButton = args.mid_button;

			m_application->OnMouseUp(btnState, args.pos.x, args.pos.y);
		});
	}

	int NanaWindow::Run()
	{
		nana::exec();

		return 0;
	}

	void NanaWindow::Show()
	{
		m_form->show();
	}
}
