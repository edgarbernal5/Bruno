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
		WindowsGameHost* m_pHost;
		HWND m_pHwnd;

		std::string m_csScreenDeviceName;
		int m_iPrevClientWidth;
		int m_iPrevClientHeight;
		bool m_bFullScreen;
		bool m_bInDeviceTransition;
		bool m_bInSizeMove;
		bool m_bInSuspend;
		bool m_bMinimized;

		// Windows procedure
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	};

}