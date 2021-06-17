#pragma once

#include "TrioApiRequisites.h"
#include <vector>
#include <string>

namespace BrunoEngine
{
	struct Screen
	{
		HMONITOR Monitor;
		//HDC m_pHdc;

		Screen();
		Screen(HMONITOR pHandle, HDC pHdc);
		
		struct MonitorEnumCallback
		{
			std::vector<Screen> m_screens;
			bool callback(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor);
			static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
		};
		static std::vector<Screen>& GetScreens();
	private:
		bool m_primary;
		std::string m_deviceName;

		static std::vector<Screen> g_screens;
		static bool g_multiMonitorSupport;
	};
}
