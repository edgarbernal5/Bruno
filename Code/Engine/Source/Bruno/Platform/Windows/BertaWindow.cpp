#include "brpch.h"
#include "BertaWindow.h"

#include "Bruno/Core/Application.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(BertaWindow);

	BertaWindow::BertaWindow(const WindowParameters& parameters, Application* application) :
		m_parameters(parameters),
		m_application(application),
		m_form(nullptr)
	{
		m_data.Width = parameters.Width;
		m_data.Height = parameters.Height;
	}

	Berta::Form& BertaWindow::GetForm()
	{
		return *m_form;
	}

	WindowHandle BertaWindow::GetHandle()
	{
		return reinterpret_cast<WindowHandle>(m_form->NativeHandle().Handle);
	}

	void BertaWindow::Initialize()
	{
		m_form = std::make_unique<Berta::Form>(Berta::Size(m_parameters.Width, m_parameters.Height));
		m_form->SetCaption(m_parameters.Title);

		m_form->GetEvents().Resize.Connect([this](const Berta::ArgResize& args)
		{
			m_data.Height = args.NewSize.Height;
			m_data.Width = args.NewSize.Width;
			m_application->OnResize();
		});
		
		m_form->GetEvents().Activated.Connect([this](const Berta::ArgActivated& args)
		{	
			if (args.IsActivated)
			{
				m_application->OnActivated();
				return;
			}
			m_application->OnDeactivated();
		});

		m_form->GetEvents().EnterSizeMove.Connect([this](const Berta::ArgSizeMove& args)
		{
			m_application->OnResizeMoveStarted();
		});

		m_form->GetEvents().ExitSizeMove.Connect([this](const Berta::ArgSizeMove& args)
		{
			m_application->OnResizeMoveFinished();
		});

		m_form->GetEvents().MouseDown.Connect([this](const Berta::ArgMouse& args)
		{
			MouseButtonState btnState{};
			btnState.LeftButton = args.ButtonState.LeftButton;
			btnState.RightButton = args.ButtonState.RightButton;
			btnState.MiddleButton = args.ButtonState.MiddleButton;

			m_application->OnMouseDown(btnState, args.Position.X, args.Position.Y);
		});

		m_form->GetEvents().MouseMove.Connect([this](const Berta::ArgMouse& args)
		{
			MouseButtonState btnState{};
			btnState.LeftButton = args.ButtonState.LeftButton;
			btnState.RightButton = args.ButtonState.RightButton;
			btnState.MiddleButton = args.ButtonState.MiddleButton;

			m_application->OnMouseMove(btnState, args.Position.X, args.Position.Y);
		});

		m_form->GetEvents().MouseUp.Connect([this](const Berta::ArgMouse& args)
		{
			MouseButtonState btnState{};
			btnState.LeftButton = args.ButtonState.LeftButton;
			btnState.RightButton = args.ButtonState.RightButton;
			btnState.MiddleButton = args.ButtonState.MiddleButton;

			m_application->OnMouseUp(btnState, args.Position.X, args.Position.Y);
		});
	}

	int BertaWindow::Run()
	{
		m_form->Exec();

		return 0;
	}

	void BertaWindow::Show()
	{
		m_form->Show();
	}
}
