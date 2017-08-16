#pragma once

#include "TrioAPI.h"

#include <string>
#include "../Event.h"
#include "Rectangles.h"

namespace Cuado
{
	class GameWindow
	{
	public:
		virtual ~GameWindow() {}
		virtual void BeginScreenDeviceChange(bool willBeFullScreen) = 0;

		virtual void EndScreenDeviceChange(std::string screenDeviceName, int clientWidth, int clientHeight) = 0;
		
		virtual HWND GetHandle() = 0;
		virtual Rectangle GetClientBounds() = 0;

		virtual std::string& const GetScreenDeviceName() = 0;

		Event<> ClientSizeChanged;
		Event<> Paint;

		static const int DefaultClientWidth = 800;
		static const int DefaultClientHeight = 600;
		

		friend class WindowsGameHost;
	protected:
		Event<> Activated;
		Event<> Deactivated;

		int m_clientWidth;
		int m_clientHeight;

		int m_prevClientWidth;
		int m_prevClientHeight;
	};
}