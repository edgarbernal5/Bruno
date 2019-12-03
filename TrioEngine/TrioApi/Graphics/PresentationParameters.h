#pragma once

#include "TrioApiRequisites.h"

#include "DepthFormat.h"
#include "PresentInterval.h"
#include "RenderTargetUsage.h"
#include "SurfaceFormat.h"

namespace TrioEngine
{
	struct TRIO_API_EXPORT PresentationParameters
	{
		PresentationParameters();
		~PresentationParameters();

		inline SurfaceFormat GetBackBufferFormat() const { return m_backBufferFormat; }

		inline RenderTargetUsage GetRenderTargetUsage() const { return m_usage; }

		inline int GetBackBufferWidth() const { return m_backBufferWidth; }

		inline int GetBackBufferHeight() const { return m_backBufferHeight; }

		inline DepthFormat GetDepthStencilFormat() const { return m_depthStencilFormat; }

		inline bool GetIsFullScreen() const { return m_fullScreen; }

		inline int GetMultiSampleCount() { return m_multiSampleCount; }

		inline HWND GetHostHWND() const { return m_deviceWindowHandle; }
		inline PresentInterval GetPresentInterval() const { return m_presentInternal; }

		inline void SetHostHWND(HWND host) { m_deviceWindowHandle = host; }
		inline void setSurfaceFormat(SurfaceFormat format) { m_backBufferFormat = format; }
		inline void setBackBufferWidth(int width) { m_backBufferWidth = width; }
		inline void setBackBufferHeight(int height) { m_backBufferHeight = height; }
		inline void setDepthStencilFormat(DepthFormat format) { m_depthStencilFormat = format; }
		inline void SetIsFullScreen(bool isfull) { m_fullScreen = isfull; }
		inline void SetPresentInterval(PresentInterval interval) { m_presentInternal = interval; }
		inline void SetRenderTargetUsage(RenderTargetUsage usage) { m_usage = usage; }

		inline void SetMultiSampleCount(int multiSampleCount) { m_multiSampleCount = multiSampleCount; }

		bool operator ==(const PresentationParameters &other) const
		{
			return m_backBufferFormat == other.m_backBufferFormat &&
				m_depthStencilFormat == other.m_depthStencilFormat &&
				m_backBufferHeight == other.m_backBufferHeight &&
				m_backBufferWidth == other.m_backBufferWidth &&
				m_multiSampleCount == other.m_multiSampleCount &&
				m_fullScreen == other.m_fullScreen &&
				m_usage == other.m_usage;
		}
	private:
		void Clear();

		SurfaceFormat				m_backBufferFormat;
		DepthFormat					m_depthStencilFormat;

		int							m_backBufferHeight, m_backBufferWidth;
		int							m_multiSampleCount;

		bool						m_fullScreen;

		HWND						m_deviceWindowHandle;

		PresentInterval				m_presentInternal;
		RenderTargetUsage			m_usage;
	};
}
