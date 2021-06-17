#include "stdafx.h"
#include "Screen.h"

namespace TrioEngine
{
	std::vector<Screen> Screen::g_screens;
	bool Screen::g_multiMonitorSupport;

	Screen::Screen() : 
		Monitor(nullptr), m_primary(false)
	{

	}

	Screen::Screen(HMONITOR pMonitor, HDC pHdc) :
		Monitor(pMonitor), m_primary(false)
	{
		if (!g_multiMonitorSupport || pMonitor == (HMONITOR)(-1163005939))
		{
			m_primary = true;
			m_deviceName = "DISPLAY";
		}
		else
		{
			MONITORINFOEXW target;
			target.cbSize = sizeof(MONITORINFOEXW);

			GetMonitorInfoW(pMonitor, &target);
			WideCharToMultiByte(CP_ACP, 0, target.szDevice, -1, &m_deviceName[0], _countof(target.szDevice), NULL, FALSE);

			m_primary = ((target.dwFlags & MONITORINFOF_PRIMARY) != 0);
		}
	}

	std::vector<Screen>& Screen::GetScreens()
	{
		if (g_screens.size() == 0)
		{
			g_multiMonitorSupport = GetSystemMetrics(SM_CMONITORS) != 0;

			if (g_multiMonitorSupport)
			{
				MonitorEnumCallback monitorCallback;
				EnumDisplayMonitors(NULL, NULL, MonitorEnumCallback::MonitorEnumProc, reinterpret_cast<LPARAM>(&monitorCallback));

				if (monitorCallback.m_screens.size() > 0)
				{
					g_screens = monitorCallback.m_screens;
				}
				else
				{
					g_screens.push_back(Screen((HMONITOR)(-1163005939), 0));
				}
			}
			else
			{
				g_screens.push_back(Screen((HMONITOR)(-1163005939), 0));
			}
		}
		return g_screens;
	}

	BOOL CALLBACK Screen::MonitorEnumCallback::MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
	{
		reinterpret_cast<Screen::MonitorEnumCallback*>(dwData)->callback(hMonitor, hdcMonitor, lprcMonitor);
		return TRUE;
	}

	bool Screen::MonitorEnumCallback::callback(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor)
	{
		m_screens.push_back(Screen(hMonitor, hdcMonitor));
		return true;
	}
}