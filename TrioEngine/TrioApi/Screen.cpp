#include "stdafx.h"
#include "Screen.h"

namespace TrioEngine
{
	std::vector<Screen> Screen::g_vScreens;
	bool Screen::g_bMultiMonitorSupport;

	Screen::Screen() : 
		m_pMonitor(nullptr), m_bPrimary(false)
	{

	}

	Screen::Screen(HMONITOR pMonitor, HDC pHdc) :
		m_pMonitor(pMonitor), m_bPrimary(false)
	{
		if (!g_bMultiMonitorSupport || pMonitor == (HMONITOR)(-1163005939))
		{
			m_bPrimary = true;
			m_csDeviceName = "DISPLAY";
		}
		else
		{
			MONITORINFOEXW target;
			target.cbSize = sizeof(MONITORINFOEXW);

			GetMonitorInfoW(pMonitor, &target);
			WideCharToMultiByte(CP_ACP, 0, target.szDevice, -1, &m_csDeviceName[0], _countof(target.szDevice), NULL, FALSE);

			m_bPrimary = ((target.dwFlags & MONITORINFOF_PRIMARY) != 0);
		}
	}

	std::vector<Screen>& Screen::GetScreens()
	{
		if (g_vScreens.size() == 0)
		{
			g_bMultiMonitorSupport = GetSystemMetrics(SM_CMONITORS) != 0;

			if (g_bMultiMonitorSupport)
			{
				MonitorEnumCallback monitorCallback;
				EnumDisplayMonitors(NULL, NULL, MonitorEnumCallback::MonitorEnumProc, reinterpret_cast<LPARAM>(&monitorCallback));

				if (monitorCallback.m_vScreens.size() > 0)
				{
					g_vScreens = monitorCallback.m_vScreens;
				}
				else
				{
					g_vScreens.push_back(Screen((HMONITOR)(-1163005939), 0));
				}
			}
			else
			{
				g_vScreens.push_back(Screen((HMONITOR)(-1163005939), 0));
			}
		}
		return g_vScreens;
	}

	BOOL CALLBACK Screen::MonitorEnumCallback::MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
	{
		reinterpret_cast<Screen::MonitorEnumCallback*>(dwData)->callback(hMonitor, hdcMonitor, lprcMonitor);
		return TRUE;
	}

	bool Screen::MonitorEnumCallback::callback(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor)
	{
		m_vScreens.push_back(Screen(hMonitor, hdcMonitor));
		return true;
	}
}