#pragma once

#if BR_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#include <Bruno/Core/KeyCodes.h>

namespace Bruno
{
	struct GameWindowParameters
	{
		std::string Title;
		uint32_t Width = 800;
		uint32_t Height = 600;
	};

#if BR_PLATFORM_WINDOWS
	using WindowHandle = HWND;
#else
	using WindowHandle = void*;
#endif

	class GameWindow
	{
	public:
		virtual ~GameWindow() {}

		virtual WindowHandle GetHandle() = 0;
		virtual void Initialize() = 0;
		virtual int Run() = 0;

		virtual uint32_t GetWidth() const;
		virtual uint32_t GetHeight() const;

		virtual void Show() = 0;

	protected:
		struct WindowData
		{
			uint32_t Width;
			uint32_t Height;
		};

		WindowData m_data;
	};
}

