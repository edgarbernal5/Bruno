#pragma once

#include "TrioApiDecl.h"
#include <vector>
#include <string>

namespace TrioEngine
{
	struct Screen
	{
		HMONITOR m_pMonitor;
		//HDC m_pHdc;

		Screen();
		Screen(HMONITOR pHandle, HDC pHdc);
		
		struct MonitorEnumCallback
		{
			std::vector<Screen> m_vScreens;
			bool callback(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor);
			static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
		};
		static std::vector<Screen>& GetScreens();
	private:
		bool m_bPrimary;
		std::string m_csDeviceName;

		static std::vector<Screen> g_vScreens;
		static bool g_bMultiMonitorSupport;
	};
}
