#pragma once

#include "Bruno/Core/Application.h"

#include <Berta/Controls/MenuBar.h>
#include <Berta/GUI/Layout.h>
#include <Berta/Controls/Panel.h>
#include <mutex>

#include "Pipeline/GameContentBuilder.h"

namespace Bruno
{
	class BertaWindow;
	class GraphicsDevice;

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
		Berta::MenuBar		m_menubar;
		Berta::Layout			m_layout;

		std::shared_ptr<GraphicsDevice>	m_device;
		GameContentBuilder	m_contentBuilder;
	};
}