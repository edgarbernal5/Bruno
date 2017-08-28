#pragma once

#include "TrioAPI.h"
#include "stdafx.h"

#include "GameWindow.h"
#include "WindowsGameHost.h"

namespace Cuado
{
	class TRIOAPI_DLL WindowsGameHost;

	class TRIOAPI_DLL WindowsGameWindow : public GameWindow
	{
	public:
		WindowsGameWindow(WindowsGameHost* host);
		~WindowsGameWindow();

		void BeginScreenDeviceChange(bool willBeFullScreen);
		void EndScreenDeviceChange(std::string screenDeviceName, int clientWidth, int clientHeight);

		Rectangle			GetClientBounds();
		HWND				GetHandle();
		std::string& const	GetScreenDeviceName();

		Event<> Resume;
		Event<> Suspend;

		friend class WindowsGameHost;
	private:
		WindowsGameHost* m_host;
		HWND m_hWnd;
		HINSTANCE m_hInstance;

		std::string m_screenDeviceName;
		bool m_bFullScreen;
		bool m_bMinimized;
		bool m_bInSizeMove;
		bool m_bInSuspend;
		bool m_bInDeviceTransition;

		bool GenerateWindow();

		// Windows procedure
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		
	};

}