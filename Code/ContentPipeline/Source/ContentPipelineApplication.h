#pragma once

#include <Bruno/Core/Application.h>

#include <nana/gui/widgets/menubar.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <mutex>

#include "Pipeline/GameContentBuilder.h"

namespace Bruno
{
	class NanaWindow;

	class ContentPipelineApplication : public Application
	{
	public:
		ContentPipelineApplication(const ApplicationParameters& parameters);
		~ContentPipelineApplication();
		
	protected:
		void InitializeUI();
		void OnInitialize() override;
		void OnInitializeWindow(const WindowParameters& windowParameters) override;
		void OnRun() override;

	private:
		nana::menubar		m_menubar;
		nana::place			m_place;

		GameContentBuilder	m_contentBuilder;
	};
}