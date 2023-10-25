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

		virtual void OnTick();

		friend class WindowsGameWindow;
		friend class NanaGameWindow;
	protected:
		virtual void OnInitialize() override;
		virtual void OnInitializeWindow(const GameWindowParameters& windowParameters) = 0;
		virtual void OnRun() override;

		virtual void OnGameLoop(const GameTimer& timer) = 0;

		virtual void OnActivated() override;
		virtual void OnDeactivated() override;
		virtual void OnResizeMoveStarted() override;
		virtual void OnResizeMoveFinished() override;

		GameTimer m_timer;
		std::shared_ptr<GraphicsDevice> m_device;

		int m_framesThisSecond = 0;
		int m_framesPerSecond = 0;
		float m_elapsedTime = 0.0f;
	private:
		bool m_gamePaused = false;

	};

}
