#include "stdafx.h"
#include "WindowsGameWindow.h"

#include "WindowsGameHost.h"
#include "Game.h"

#include <iostream>

namespace Cuado
{

	WindowsGameWindow::WindowsGameWindow(WindowsGameHost* host) :
		m_hWnd(nullptr),
		m_bInDeviceTransition(false),
		m_bMinimized(false),
		m_bInSizeMove(false),
		m_bInSuspend(false),
		m_bFullScreen(false)
	{
		m_prevClientWidth = m_prevClientHeight = 0;
		
		m_hInstance = host->GetGame()->m_hInstance;
		if (m_hInstance == NULL)
			m_hInstance = GetModuleHandle(NULL);
	}


	WindowsGameWindow::~WindowsGameWindow()
	{
	}


	void WindowsGameWindow::BeginScreenDeviceChange(bool willBeFullScreen)
	{
		m_bInDeviceTransition = true;
		m_prevClientWidth = m_clientWidth;
		m_prevClientHeight = m_clientHeight;
	}

	void WindowsGameWindow::EndScreenDeviceChange(std::string screenDeviceName, int clientWidth, int clientHeight)
	{
		m_bInDeviceTransition = false;

		m_clientWidth = clientWidth;
		m_clientHeight = clientHeight;
	}

	bool WindowsGameWindow::GenerateWindow()
	{
		// Register class
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = m_hInstance;
		wcex.hIcon = LoadIcon(m_hInstance, L"IDI_ICON");
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = nullptr;
		wcex.lpszClassName = L"TrioEngine";
		wcex.hIconSm = LoadIcon(wcex.hInstance, L"IDI_ICON");
		if (!RegisterClassEx(&wcex))
			return FALSE;

		// Compute window rectangle dimensions based on requested client area dimensions.
		RECT rc = { 0, 0, DefaultClientWidth, DefaultClientHeight };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
		int width = rc.right - rc.left;
		int height = rc.bottom - rc.top;

		std::wstring m_MainWndCaption(L"Game");

		m_hWnd = CreateWindowEx(0, L"TrioEngine", m_MainWndCaption.c_str(), WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, m_hInstance,
			nullptr);
		
		if (!m_hWnd)
		{
			//MessageBox(0, L"CreateWindow Failed.", 0, 0);
			return false;
		}
		SetWindowLongPtr(m_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		GetClientRect(m_hWnd, &rc);
		m_clientWidth = rc.right - rc.left;
		m_clientHeight = rc.bottom - rc.top;

		ShowWindow(m_hWnd, SW_SHOW);
		UpdateWindow(m_hWnd);

		return true;
	}

	LRESULT CALLBACK WindowsGameWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		WindowsGameWindow* window = reinterpret_cast<WindowsGameWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

		PAINTSTRUCT ps;
		HDC hdc;

		switch (message)
		{
		case WM_PAINT:
			//window->Paint();
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;

		case WM_SIZE:
			if (wParam == SIZE_MINIMIZED)
			{
				if (!window->m_bMinimized)
				{
					window->m_bMinimized = true;
					if (!window->m_bInSuspend) {
						std::cout << "OnSuspending" << std::endl;
						window->Suspend();
					}
					window->m_bInSuspend = true;
				}
			}
			else if (window->m_bMinimized) {
				window->m_bMinimized = false;
				if (window->m_bInSuspend) {
					std::cout << "OnResuming" << std::endl;
					window->Resume();
				}
				window->m_bInSuspend = false;
			}
			else if (!window->m_bInSizeMove) {
				std::cout << "ClientSizeChanged" << std::endl;
				window->ClientSizeChanged();
			}
			break;

		case WM_ENTERSIZEMOVE:
			window->m_bInSizeMove = true;

			window->BeginScreenDeviceChange(false);
			break;

		case WM_EXITSIZEMOVE:
			window->m_bInSizeMove = false;

			RECT rc;
			GetClientRect(hWnd, &rc);

			window->EndScreenDeviceChange("", rc.right - rc.left, rc.bottom - rc.top);
			if (window->m_prevClientWidth != window->m_clientWidth || window->m_prevClientHeight != window->m_clientHeight){
				
				std::cout << "ClientSizeChanged" << std::endl;
				window->ClientSizeChanged();
			}			
			break;

		case WM_GETMINMAXINFO:
		{
			auto info = reinterpret_cast<MINMAXINFO*>(lParam);
			info->ptMinTrackSize.x = 320;
			info->ptMinTrackSize.y = 200;
		}
			break;

		case WM_ACTIVATEAPP:
			if (window)
			{
				if (wParam)
				{
					std::cout << "Activated" << std::endl;
					window->Activated();
				}
				else
				{
					std::cout << "Deactivated" << std::endl;
					window->Deactivated();
				}
			}
			break;

		case WM_POWERBROADCAST:
			switch (wParam)
			{
			case PBT_APMQUERYSUSPEND:
				if (!window->m_bInSuspend)
				{
					std::cout << "OnSuspending" << std::endl;
					window->Suspend();
				}

				window->m_bInSuspend = true;
				return true;

			case PBT_APMRESUMESUSPEND:
				if (!window->m_bMinimized)
				{
					if (window->m_bInSuspend)
					{
						std::cout << "OnResuming" << std::endl;
						window->Resume();
					}
					window->m_bInSuspend = false;
				}
				return true;
			}
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;


		case WM_INPUT:
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MOUSEWHEEL:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_MOUSEHOVER:
			break;


		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			break;

		case WM_SYSKEYDOWN:
			break;
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	Rectangle WindowsGameWindow::GetClientBounds()
	{
		RECT rcWindowClient;
		GetClientRect(m_hWnd, &rcWindowClient);

		Rectangle rect;
		rect.X = rcWindowClient.left;
		rect.Y = rcWindowClient.top;
		rect.Height = rcWindowClient.bottom - rcWindowClient.top;
		rect.Width = rcWindowClient.right - rcWindowClient.left;
		return rect;
	}

	HWND WindowsGameWindow::GetHandle()
	{
		return m_hWnd;
	}

	std::string& const WindowsGameWindow::GetScreenDeviceName()
	{
		MONITORINFOEXW target;
		target.cbSize = sizeof(MONITORINFOEXW);

		//create a monitor info struct to store the data to
		HMONITOR Hmon = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY);
		//create a handle to the main monitor
		//(should start at top left of screen with (0,0) as apposed to other monitors i believe)
		//if i could gather aditional info on what monitors are available that might be           useful
		GetMonitorInfoW(Hmon, &target);
		
		size_t size = wcslen(target.szDevice);
		
		std::wstring wstr(target.szDevice, target.szDevice+size);
		m_screenDeviceName = std::string(wstr.begin(), wstr.end());
		return m_screenDeviceName;
	}

}