#pragma once

#include "Bruno/Core/Base.h"
#include "Bruno/Core/GameTimer.h"

#include "Bruno/Core/GameWindow.h"
#include "Bruno/Core/KeyCodes.h"

#include <cstdint>
#include <string>
#include <memory>

namespace Bruno
{
	struct GameParameters
	{
		std::string Name = "Bruno";
		uint32_t WindowWidth = 800;
		uint32_t WindowHeight = 600;
	};

	class GraphicsDevice;

	class Game
	{
	public:
		Game(const GameParameters& parameters);
		virtual ~Game();

		inline GraphicsDevice* GetDevice() { return m_device.get(); }
		void Initialize();
		void Run();

		static Game* GetInstance() { return g_instance; }

		virtual void OnTick();

		friend class WindowsGameWindow;
		friend class NanaGameWindow;
	protected:
		std::unique_ptr<GameWindow> m_gameWindow;
		GameParameters m_parameters;
		GameTimer m_timer;
		std::shared_ptr<GraphicsDevice> m_device;

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

		int m_framesThisSecond = 0;
		int m_framesPerSecond = 0;
		float m_elapsedTime = 0.0f;
	private:
		static Game* g_instance;

		bool m_gamePaused = false;
	};

	Game* CreateGame(int argc, char** argv);
}
