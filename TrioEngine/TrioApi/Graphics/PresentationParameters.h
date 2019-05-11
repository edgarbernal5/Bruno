#pragma once

#include "TrioApiDecl.h"

#include "DepthFormat.h"
#include "PresentInterval.h"
#include "RenderTargetUsage.h"
#include "SurfaceFormat.h"

namespace TrioEngine
{
	struct TRIOAPI_DECL PresentationParameters
	{
		PresentationParameters();
		~PresentationParameters();

		inline SurfaceFormat GetBackBufferFormat() const { return m_eBackBufferFormat; }

		inline RenderTargetUsage GetRenderTargetUsage() const { return m_eUsage; }

		inline int GetBackBufferWidth() const { return m_iBackBufferWidth; }

		inline int GetBackBufferHeight() const { return m_iBackBufferHeight; }

		inline DepthFormat GetDepthStencilFormat() const { return m_eDepthStencilFormat; }

		inline bool GetIsFullScreen() const { return m_bFullScreen; }

		inline int GetMultiSampleCount() { return m_iMultiSampleCount; }

		inline HWND GetHostHWND() const { return m_pDeviceWindowHandle; }
		inline PresentInterval GetPresentInterval() const { return m_ePresentInternal; }

		inline void SetHostHWND(HWND host) { m_pDeviceWindowHandle = host; }
		inline void setSurfaceFormat(SurfaceFormat format) { m_eBackBufferFormat = format; }
		inline void setBackBufferWidth(int width) { m_iBackBufferWidth = width; }
		inline void setBackBufferHeight(int height) { m_iBackBufferHeight = height; }
		inline void setDepthStencilFormat(DepthFormat format) { m_eDepthStencilFormat = format; }
		inline void SetIsFullScreen(bool isfull) { m_bFullScreen = isfull; }
		inline void SetPresentInterval(PresentInterval interval) { m_ePresentInternal = interval; }
		inline void SetRenderTargetUsage(RenderTargetUsage usage) { m_eUsage = usage; }

		inline void SetMultiSampleCount(int multiSampleCount) { m_iMultiSampleCount = multiSampleCount; }

		bool operator ==(const PresentationParameters &other) const
		{
			return m_eBackBufferFormat == other.m_eBackBufferFormat &&
				m_eDepthStencilFormat == other.m_eDepthStencilFormat &&
				m_iBackBufferHeight == other.m_iBackBufferHeight &&
				m_iBackBufferWidth == other.m_iBackBufferWidth &&
				m_iMultiSampleCount == other.m_iMultiSampleCount &&
				m_bFullScreen == other.m_bFullScreen &&
				m_eUsage == other.m_eUsage;
		}
	private:
		void Clear();

		SurfaceFormat				m_eBackBufferFormat;
		DepthFormat					m_eDepthStencilFormat;

		int							m_iBackBufferHeight, m_iBackBufferWidth;
		int							m_iMultiSampleCount;

		bool						m_bFullScreen;

		HWND						m_pDeviceWindowHandle;

		PresentInterval				m_ePresentInternal;
		RenderTargetUsage			m_eUsage;
	};
}
