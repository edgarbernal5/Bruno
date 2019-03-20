#pragma once

#include "TrioApiDecl.h"
#include "Event.h"
#include "Math\Rectangle.h"

#include <string>

namespace Vago
{
	class GameWindow
	{
	public:
		virtual ~GameWindow() {}

		//Starts a device transition (windowed to full screen or vice versa).
		virtual void BeginScreenDeviceChange(bool willBeFullScreen) = 0;

		virtual bool LoadWindow() = 0;

		//Completes a device transition.
		virtual void EndScreenDeviceChange(std::string screenDeviceName, int clientWidth, int clientHeight) = 0;

		virtual inline Rectangle GetClientBounds() = 0;
		virtual HWND GetHandle() = 0;

		virtual const char* GetScreenDeviceName() = 0;

		Event<> Activated;
		Event<> ClientSizeChanged;
		Event<> Deactivated;
		Event<> Paint;
		Event<> Resume;
		Event<> Suspend;

		static const int DefaultClientWidth = 800;
		static const int DefaultClientHeight = 600;

	protected:
	};
}
