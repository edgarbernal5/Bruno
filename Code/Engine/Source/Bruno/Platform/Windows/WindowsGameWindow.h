#pragma once

#include "Bruno/Core/GameWindow.h"

#ifdef BR_PLATFORM_WINDOWS

#include <Windows.h>

#endif

namespace Bruno
{
	class Game;

	class WindowsGameWindow : public GameWindow
	{
	public:
		WindowsGameWindow(const GameWindowParameters& parameters, Game* game);
		virtual ~WindowsGameWindow();

		virtual WindowHandle GetHandle() override;
		virtual void Initialize() override;
		virtual void Run() override;

		virtual void Show() override;

	protected:
		GameWindowParameters m_parameters;
		HWND m_hwnd;
		Game* m_game;
		int m_prevClientWidth;
		int m_prevClientHeight;
		bool m_fullScreen;
		bool m_inDeviceTransition;
		bool m_inSizeMove;
		bool m_inSuspend;
		bool m_minimized;

		inline RECT GetClientBounds();
		void BeginScreenDeviceChange();
		void EndScreenDeviceChange(int clientWidth, int clientHeight);

	private:
		// Windows procedure
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	};
}

