#include "brpch.h"
#include "WindowsGameWindow.h"

#include "Bruno/Core/Game.h"
#include <iostream>

namespace Bruno
{
	//TODO: eliminar la dependencia con la clase Game. Implementar eventos!

	WindowsGameWindow::WindowsGameWindow(const GameWindowParameters& parameters, Game* game) :
		m_parameters(parameters),
		m_game(game),
		m_hwnd(nullptr),
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
		wcex.lpszClassName = L"BrunoEngineClass";
		wcex.hIconSm = LoadIconW(wcex.hInstance, L"IDI_ICON");
		if (!RegisterClassExW(&wcex))
			return;

		// Compute window rectangle dimensions based on requested client area dimensions.
		RECT windowRect = { 0, 0, (LONG)m_parameters.Width, (LONG)m_parameters.Height };
		AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

		std::wstring m_mainWndTitle(m_parameters.Title.begin(), m_parameters.Title.end());

		m_hwnd = CreateWindowEx
		(
			0,
			L"BrunoEngineClass", 
			m_mainWndTitle.c_str(), 
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, 
			CW_USEDEFAULT, 
			windowRect.right - windowRect.left, 
			windowRect.bottom - windowRect.top,
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
		case WM_SYSKEYUP:
			break;

		case WM_SYSKEYDOWN:
			break;
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}