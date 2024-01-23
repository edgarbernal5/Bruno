#include "brpch.h"
#include "WindowsGameWindow.h"

#include "Bruno/Core/Game.h"
#include <iostream>

namespace Bruno
{
	BR_RTTI_DEFINITIONS(WindowsGameWindow);
	//TODO: eliminar la dependencia con la clase Game. Implementar eventos!

	WindowsGameWindow::WindowsGameWindow(const WindowParameters& parameters, Game* game) :
		m_parameters(parameters),
		m_game(game),
		m_hwnd(nullptr),
		m_hModuleInstance(nullptr),
		m_fullScreen(false),
		m_prevClientWidth(0),
		m_prevClientHeight(0),
		m_inDeviceTransition(false),
		m_inSizeMove(false),
		m_inSuspend(false),
		m_minimized(false)
	{
	}

	WindowsGameWindow::~WindowsGameWindow()
	{
	}

	void WindowsGameWindow::BeginScreenDeviceChange()
	{
		m_inDeviceTransition = true;

		auto rect = GetClientBounds();
		m_prevClientWidth = rect.right - rect.left;
		m_prevClientHeight = rect.bottom - rect.top;
	}

	WindowHandle WindowsGameWindow::GetHandle()
	{
		return m_hwnd;
	}

	void WindowsGameWindow::Initialize()
	{
		SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
		HINSTANCE hInstance = GetModuleHandle(NULL);

		// Register class
		WNDCLASSEXW wcex = {};
		wcex.cbSize = sizeof(WNDCLASSEXW);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIconW(hInstance, L"IDI_ICON");
		wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
		wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
		wcex.lpszClassName = ApplicationClassName;
		wcex.hIconSm = LoadIconW(wcex.hInstance, L"IDI_ICON");
		if (!RegisterClassExW(&wcex))
		{
			BR_CORE_ERROR << "RegisterClassExW Failed." << std::endl;
			return;
		}
				
		UINT dpi = GetDpiForSystem(); //GetDpiForWindow(m_hwnd);
		float scaling_factor = static_cast<float>(dpi) / 96.0f;
		// Actually set the appropriate window size
		RECT scaledWindowRect;
		scaledWindowRect.left = 0;
		scaledWindowRect.top = 0;
		scaledWindowRect.right = static_cast<LONG>(m_parameters.Width * scaling_factor);
		scaledWindowRect.bottom = static_cast<LONG>(m_parameters.Height * scaling_factor);

		if (!AdjustWindowRectExForDpi(&scaledWindowRect, WS_OVERLAPPEDWINDOW, false, 0, dpi))
		{
			BR_CORE_ERROR << "AdjustWindowRectExForDpi Failed." << std::endl;
			return;
		}

		std::wstring m_mainWndTitle(m_parameters.Title.begin(), m_parameters.Title.end());

		m_hwnd = CreateWindowEx
		(
			0,
			ApplicationClassName,
			m_mainWndTitle.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			scaledWindowRect.right - scaledWindowRect.left,
			scaledWindowRect.bottom - scaledWindowRect.top,
			nullptr,			// We have no parent window.
			nullptr,			// We aren't using menus.
			hInstance,
			this
		);

		if (!m_hwnd)
		{
			BR_CORE_ERROR << "CreateWindow Failed." << std::endl;
			return;
		}

		m_hModuleInstance = hInstance;
	}

	int WindowsGameWindow::Run()
	{
		MSG msg = { 0 };

		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		DestroyWindow(m_hwnd);
		m_hwnd = nullptr;

		UnregisterClass(ApplicationClassName, m_hModuleInstance);
		m_hModuleInstance = nullptr;

		int returnCode = (int)msg.wParam;
		return returnCode;
	}

	void WindowsGameWindow::Show()
	{
		ShowWindow(m_hwnd, SW_SHOW);
	}

	inline RECT WindowsGameWindow::GetClientBounds()
	{
		RECT rcWindowClient;
		GetClientRect(m_hwnd, &rcWindowClient);

		/*Rectangle rect;
		rect.x = rcWindowClient.left;
		rect.y = rcWindowClient.top;
		rect.height = rcWindowClient.bottom - rcWindowClient.top;
		rect.width = rcWindowClient.right - rcWindowClient.left;*/
		return rcWindowClient;
	}

	void WindowsGameWindow::EndScreenDeviceChange(int clientWidth, int clientHeight)
	{
		m_inDeviceTransition = false;
	}

	LRESULT CALLBACK WindowsGameWindow::WndProc(HWND hWnd, uint32_t message, WPARAM wParam, LPARAM lParam)
	{
		//TODO: eliminar la dependencia con la clase Game. Implementar eventos!
		WindowsGameWindow* window = reinterpret_cast<WindowsGameWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		//Actualizar esto
		//https://github.com/walbourn/directxtk-samples/blob/main/SimpleSampleWin32/Main.cpp

		switch (message)
		{

		case WM_CREATE:
		{
			LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));

			return 0;
		}
		case WM_PAINT:
		{
			//std::cout << "Native form Paint" << std::endl;
			//ver codigo nana (Bedrock_WIN32_WindowProc, bedrock_windows.cpp)

			/*PAINTSTRUCT ps;
			std::ignore = BeginPaint(hWnd, &ps);
			////window->Paint();
			EndPaint(hWnd, &ps);
			break;*/
			if (window)
			{
				window->m_game->OnTick();
			}
			return 0;
		}
		case WM_DPICHANGED:
			{
				auto r = reinterpret_cast<const RECT*>(lParam);
				auto dpi_x = HIWORD(wParam);
				auto dpi_y = LOWORD(wParam);

				::SetWindowPos(hWnd,
					NULL,
					r->left,
					r->top,
					r->right - r->left,
					r->bottom - r->top,
					SWP_NOZORDER | SWP_NOACTIVATE);
			}
			return 0;
		case WM_SIZE:
			if (window)
			{
				if (wParam == SIZE_MINIMIZED)
				{
					if (!window->m_minimized)
					{
						window->m_minimized = true;
						if (!window->m_inSuspend)
						{
							BR_CORE_TRACE << "OnSuspending" << std::endl;
							//window->Suspend();
						}
						window->m_inSuspend = true;
					}
				}
				else if (window->m_minimized)
				{
					window->m_minimized = false;
					if (window->m_inSuspend)
					{
						BR_CORE_TRACE << "OnResuming" << std::endl;
						//window->Resume();
					}
					window->m_inSuspend = false;
				}
				else if (!window->m_inSizeMove)
				{
					window->m_data.Height = HIWORD(lParam);
					window->m_data.Width = LOWORD(lParam);

					BR_CORE_TRACE << "OnResize" << std::endl;
					window->m_game->OnResize();
				}
			}
			break;

		case WM_ENTERSIZEMOVE:
			BR_CORE_TRACE << "Native enter size move" << std::endl;
			window->m_inSizeMove = true;
			window->BeginScreenDeviceChange();
			window->m_game->OnResizeMoveStarted();
			break;

		case WM_EXITSIZEMOVE:
		{
			BR_CORE_TRACE << "Native exit size move" << std::endl;
			window->m_inSizeMove = false;

			RECT rc;
			GetClientRect(hWnd, &rc);

			window->EndScreenDeviceChange(rc.right - rc.left, rc.bottom - rc.top);
			int clientWidth = rc.right - rc.left;
			int clientHeight = rc.bottom - rc.top;

			if (window->m_prevClientWidth != clientWidth || window->m_prevClientHeight != clientHeight)
			{
				BR_CORE_TRACE << "OnResize" << std::endl;
				window->m_data.Height = clientHeight;
				window->m_data.Width = clientWidth;

				window->m_game->OnResize();
			}
			window->m_game->OnResizeMoveFinished();
			break;
		}

		case WM_GETMINMAXINFO:
		{
			auto info = reinterpret_cast<MINMAXINFO*>(lParam);
			info->ptMinTrackSize.x = 320;
			info->ptMinTrackSize.y = 200;
			break;
		}

		case WM_ACTIVATEAPP:
			if (window)
			{
				if (wParam)
				{
					BR_CORE_TRACE << "Activated" << std::endl;
					window->m_game->OnActivated();
				}
				else
				{
					BR_CORE_TRACE << "Deactivated" << std::endl;
					window->m_game->OnDeactivated();
				}
			}
			break;

		case WM_POWERBROADCAST:
			switch (wParam)
			{
			case PBT_APMQUERYSUSPEND:
				if (!window->m_inSuspend)
				{
					BR_CORE_TRACE << "OnSuspending" << std::endl;
					//window->Suspend();
				}

				window->m_inSuspend = true;
				return true;

			case PBT_APMRESUMESUSPEND:
				if (!window->m_minimized)
				{
					if (window->m_inSuspend)
					{
						BR_CORE_TRACE << "OnResuming" << std::endl;
						//window->Resume();
					}
					window->m_inSuspend = false;
				}
				return true;
			}
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		{
			MouseButtonState btnState{};
			btnState.LeftButton = (wParam & MK_LBUTTON) != 0;
			btnState.RightButton = (wParam & MK_RBUTTON) != 0;
			btnState.MiddleButton = (wParam & MK_MBUTTON) != 0;

			int x = ((int)(short)LOWORD(lParam));
			int y = ((int)(short)HIWORD(lParam));

			window->m_game->OnMouseDown(btnState, x, y);
			break;
		}
		case WM_MOUSEMOVE:
		{
			MouseButtonState btnState{};
			btnState.LeftButton = (wParam & MK_LBUTTON) != 0;
			btnState.RightButton = (wParam & MK_RBUTTON) != 0;
			btnState.MiddleButton = (wParam & MK_MBUTTON) != 0;

			int x = ((int)(short)LOWORD(lParam));
			int y = ((int)(short)HIWORD(lParam));

			window->m_game->OnMouseMove(btnState, x, y);
			break;
		}
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		{
			MouseButtonState btnState{};
			btnState.LeftButton = (wParam & MK_LBUTTON) != 0;
			btnState.RightButton = (wParam & MK_RBUTTON) != 0;
			btnState.MiddleButton = (wParam & MK_MBUTTON) != 0;

			int x = ((int)(short)LOWORD(lParam));
			int y = ((int)(short)HIWORD(lParam));

			window->m_game->OnMouseUp(btnState, x, y);
			break;
		}
		case WM_MOUSEWHEEL:
		{
			int zoomDelta = ((int)(short)HIWORD(wParam));
			short keyStates = (short)LOWORD(wParam);

			MouseButtonState btnState{};
			btnState.LeftButton = (keyStates & MK_LBUTTON) != 0;
			btnState.RightButton = (keyStates & MK_RBUTTON) != 0;
			btnState.MiddleButton = (keyStates & MK_MBUTTON) != 0;

			int x = ((int)(short)LOWORD(lParam));
			int y = ((int)(short)HIWORD(lParam));

			// Convert the screen coordinates to client coordinates.
			POINT screenToClientPoint;
			screenToClientPoint.x = x;
			screenToClientPoint.y = y;
			::ScreenToClient(hWnd, &screenToClientPoint);

			window->m_game->OnMouseWheel(btnState, (int)screenToClientPoint.x, (int)screenToClientPoint.y, zoomDelta);
			break;
		}
		case WM_INPUT:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_MOUSEHOVER:
			break;

		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		{
			WORD vkCode = LOWORD(wParam);

			KeyboardState btnState{};
			btnState.Alt = (0 != (::GetKeyState(VK_MENU) & 0x80));
			btnState.Ctrl = (0 != (::GetKeyState(VK_CONTROL) & 0x80));
			btnState.Shift = (0 != (::GetKeyState(VK_SHIFT) & 0x80));
			WORD keyFlags = HIWORD(lParam);

			BOOL isKeyReleased = (keyFlags & KF_UP) == KF_UP;

			KeyCode keycode = static_cast<KeyCode>(vkCode);
			if (isKeyReleased)
			{
				window->m_game->OnKeyReleased(keycode, btnState);
			}
			else
			{
				window->m_game->OnKeyPressed(keycode, btnState);
			}
			break;
		}
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}