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
		m_eBackBufferFormat = SurfaceFormat::Color;
		m_eDepthStencilFormat = DepthFormat::Depth24Stencil8;

		//m_BackBufferHeight = GraphicsDeviceManager::DefaultBackBufferHeight;
		//m_BackBufferWidth = GraphicsDeviceManager::DefaultBackBufferWidth;

		m_pDeviceWindowHandle = nullptr;
		m_bFullScreen = false;

		m_iMultiSampleCount = 0;

		m_ePresentInternal = PresentInterval::Default;
		m_eUsage = RenderTargetUsage::DiscardContents;
	}
}