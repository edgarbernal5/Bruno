#pragma once

#include "GameWindow.h"

namespace TrioEngine
{
	class WindowsGameHost;

	class WindowsGameWindow : public GameWindow
	{
	public:
		WindowsGameWindow(WindowsGameHost* host);
		~WindowsGameWindow();

		//Starts a device transition (windowed to full screen or vice versa).
		void BeginScreenDeviceChange(bool willBeFullScreen);
		
		bool LoadWindow();

		//Completes a device transition.
		void EndScreenDeviceChange(std::string screenDeviceName, int clientWidth, int clientHeight);

		inline Rectangle GetClientBounds();
		HWND GetHandle();

		const char* GetScreenDeviceName();

	private:
		WindowsGameHost* m_host;
		HWND m_hwnd;

		std::string m_screenDeviceName;
		int m_prevClientWidth;
		int m_prevClientHeight;
		bool m_fullScreen;
		bool m_inDeviceTransition;
		bool m_inSizeMove;
		bool m_inSuspend;
		bool m_minimized;

		// Windows procedure
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	};

}