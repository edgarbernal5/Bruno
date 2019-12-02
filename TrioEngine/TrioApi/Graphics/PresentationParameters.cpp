#include "stdafx.h"
#include "PresentationParameters.h"

namespace TrioEngine
{
	PresentationParameters::PresentationParameters()
	{
		Clear();
	}

	PresentationParameters::~PresentationParameters()
	{
	}

	void PresentationParameters::Clear()
	{
		m_backBufferFormat = SurfaceFormat::Color;
		m_depthStencilFormat = DepthFormat::Depth24Stencil8;

		//m_BackBufferHeight = GraphicsDeviceManager::DefaultBackBufferHeight;
		//m_BackBufferWidth = GraphicsDeviceManager::DefaultBackBufferWidth;

		m_deviceWindowHandle = nullptr;
		m_fullScreen = false;

		m_multiSampleCount = 0;

		m_presentInternal = PresentInterval::Default;
		m_usage = RenderTargetUsage::DiscardContents;
	}
}