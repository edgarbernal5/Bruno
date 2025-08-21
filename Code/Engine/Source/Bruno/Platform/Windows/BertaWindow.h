#pragma once

#include "WindowsGameWindow.h"

#include "Berta/Controls/Form.h"
#include "Bruno/Core/GameTimer.h"

namespace Bruno
{
	class Application;

	class BertaWindow : public Window
	{
		BR_RTTI_DECLARATION(BertaWindow, Window);

	public:
		BertaWindow(const WindowParameters& parameters, Application* application);
		~BertaWindow() = default;

		Berta::Form& GetForm();
		virtual WindowHandle GetHandle() override;
		virtual void Initialize() override;
		virtual int Run() override;

		virtual void Show() override;

	private:
		std::unique_ptr<Berta::Form> m_form;
		WindowParameters m_parameters;
		Application* m_application;
	};
}