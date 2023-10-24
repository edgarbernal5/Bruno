#pragma once

#include "Bruno/Core/Base.h"
#include "Bruno/Core/Application.h"
#include "Bruno/Core/GameTimer.h"

#include "Bruno/Core/KeyCodes.h"

#include <cstdint>
#include <string>
#include <memory>

namespace Bruno
{
	class GraphicsDevice;

	class Game : public Application
	{
	public:
		Game(const ApplicationParameters& parameters);
		virtual ~Game();

		inline GraphicsDevice* GetDevice() { return m_device.get(); }
		virtual void Initialize() override;
		virtual void Run() override;

		virtual void OnTick();

		friend class WindowsGameWindow;
		friend class NanaGameWindow;
	protected:
		virtual void OnInitialize(const GameWindowParameters& windowParameters) = 0;

		virtual void OnResize() {}
		virtual void OnActivated();
		virtual void OnDeactivated();
		virtual void OnResizeMoveStarted();
		virtual void OnResizeMoveFinished();
		virtual void OnGameLoop(const GameTimer& timer) = 0;

		virtual void OnMouseDown(MouseButtonState btnState, int x, int y) {}
		virtual void OnMouseMove(MouseButtonState btnState, int x, int y) {}
		virtual void OnMouseUp(MouseButtonState btnState, int x, int y) {}
		virtual void OnMouseWheel(MouseButtonState btnState, int x, int y, int wheelDelta) {}
		virtual void OnKeyPressed(KeyCode key, KeyboardState state) {}
		virtual void OnKeyReleased(KeyCode key, KeyboardState state) {}

		GameTimer m_timer;
		std::shared_ptr<GraphicsDevice> m_device;

		int m_framesThisSecond = 0;
		int m_framesPerSecond = 0;
		float m_elapsedTime = 0.0f;
	private:
		bool m_gamePaused = false;

	};

}
